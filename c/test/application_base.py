from argparse import ArgumentParser
import copy
import os
import re
import signal
import subprocess
import sys
import threading


class TestApplicationBase(object):
    TEST_PASSED = 0
    TEST_FAILED = 1
    ARGUMENT_ERROR = 2
    EXECUTION_ERROR = 3
    NONZERO_EXIT = 4

    DEFAULT_ROOT_DIR = os.path.abspath(os.path.normpath(os.path.join(os.path.dirname(__file__), '..')))
    DEFAULT_COMMAND = ['%(path)s', '%(polaris_api_key)s', '%(unique_id)s']

    def __init__(self, application_name, root_dir=None):
        if root_dir is None:
            self.root_dir = self.DEFAULT_ROOT_DIR
        else:
            self.root_dir = root_dir

        self.application_name = application_name

        # Define and parse arguments.
        self.parser = ArgumentParser(usage='%(prog)s [OPTIONS]...')

        self.parser.add_argument(
            '-p', '--path', metavar='PATH',
            help="The path to the application to be run.")
        self.parser.add_argument(
            '--polaris-api-key', metavar='KEY',
            help="The Polaris API key to be used. If not set, defaults to the POLARIS_API_KEY environment variable if "
            "specified.")
        self.parser.add_argument(
            '-t', '--timeout', metavar='SEC', type=float, default=30.0,
            help="The maximum test duration (in seconds).")
        self.parser.add_argument(
            '--tool', metavar='TOOL', default='bazel',
            help="The tool used to compile the application (bazel, cmake, make), used to determine the default "
            "application path. Ignored if --path is specified.")
        self.parser.add_argument(
            '--unique-id', metavar='ID', default=application_name,
            help="The unique ID to assign to this instance. The ID will be prepended with PREFIX if --unique-id-prefix "
            "is specified.")
        self.parser.add_argument(
            '--unique-id-prefix', metavar='PREFIX',
            help="An optional prefix to prepend to the unique ID.")

        self.options = None

        self.program_args = []

        self.proc = None

    def parse_args(self):
        self.options = self.parser.parse_args()

        if self.options.polaris_api_key is None:
            self.options.polaris_api_key = os.getenv('POLARIS_API_KEY')
        if self.options.polaris_api_key is None:
            print('Error: Polaris API key not specified.')
            sys.exit(self.ARGUMENT_ERROR)

        if self.options.path is None:
            if self.options.tool == 'bazel':
                self.options.path = os.path.join(self.root_dir, 'bazel-bin/examples', self.application_name)
            elif self.options.tool == 'cmake':
                for build_dir in ('build', 'cmake_build'):
                    path = os.path.join(self.root_dir, build_dir, 'examples', self.application_name)
                    if os.path.exists(path):
                        self.options.path = path
                        break
                if self.options.path is None:
                    print('Error: Unable to locate CMake build directory.')
                    sys.exit(self.ARGUMENT_ERROR)
            elif self.options.tool == 'make':
                self.options.path = os.path.join(self.root_dir, 'examples', self.application_name)
            else:
                print('Error: Unsupported --tool value.')
                sys.exit(self.ARGUMENT_ERROR)

        if self.options.unique_id_prefix is not None:
            self.options.unique_id = self.options.unique_id_prefix + self.options.unique_id
        if len(self.options.unique_id) > 36:
            self.options.unique_id = self.options.unique_id[:36]
            print("Unique ID too long. Truncating to '%s'." % self.options.unique_id)

        return self.options

    def run(self, return_result=False):
        # Setup the command to be run.
        command = copy.deepcopy(self.DEFAULT_COMMAND)
        command.extend(self.program_args)
        api_key_standin = '%s...' % self.options.polaris_api_key[:4]
        for i in range(len(command)):
            if command[i].endswith('%(polaris_api_key)s'):
                # We temporarily replace the API key placeholder with the first 4 chars of the key before printing to
                # the console to avoid printing the actual key to the console. It will be swapped with the real key
                # below.
                command[i] = command[i].replace('%(polaris_api_key)s', api_key_standin)
            else:
                command[i] = command[i] % self.options.__dict__

        print('Executing: %s' % ' '.join(command))

        command.insert(0, 'stdbuf')
        command.insert(1, '-o0')
        for i in range(len(command)):
            if command[i].endswith(api_key_standin):
                command[i] = command[i].replace(api_key_standin, self.options.polaris_api_key)

        # Run the command.
        def preexec_function():
            # Disable forwarding of SIGINT/SIGTERM from the parent process (this script) to the child process (the
            # application under test).
            os.setpgrp()

        self.proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, encoding='utf-8',
                                     preexec_fn=preexec_function)

        # Capture SIGINT and SIGTERM and shutdown the application gracefully.
        def request_shutdown(sig, frame):
            self.stop()
            signal.signal(sig, signal.SIG_DFL)

        signal.signal(signal.SIGINT, request_shutdown)
        signal.signal(signal.SIGTERM, request_shutdown)

        # Stop the test after a max duration.
        def timeout_elapsed():
            print('Maximum test duration (%.1f sec) elapsed.' % self.options.timeout)
            self.stop()

        watchdog = threading.Timer(self.options.timeout, timeout_elapsed)
        watchdog.start()

        # Check for a pass/fail condition and forward output to the console.
        while True:
            try:
                line = self.proc.stdout.readline().rstrip('\n')
                if line != '':
                    print(line.rstrip('\n'))
                    self.on_stdout(line)
                elif self.proc.poll() is not None:
                    exit_code = self.proc.poll()
                    break
            except KeyboardInterrupt:
                print('Execution interrupted unexpectedly.')
                if return_result:
                    return self.EXECUTION_ERROR
                else:
                    sys.exit(self.EXECUTION_ERROR)

        watchdog.cancel()
        self.proc = None

        result = self.check_pass_fail(exit_code)
        if result == self.TEST_PASSED:
            print('Test result: success')
        else:
            print('Test result: FAIL')

        if return_result:
            return result
        else:
            sys.exit(result)

    def stop(self):
        if self.proc is not None:
            print('Sending shutdown request to the application.')
            self.proc.terminate()

    def check_pass_fail(self, exit_code):
        if exit_code != 0:
            print('Application exited with non-zero exit code %s.' % repr(exit_code))
            return self.NONZERO_EXIT
        else:
            return self.TEST_PASSED

    def on_stdout(self, line):
        pass


class StandardApplication(TestApplicationBase):
    """!
    @brief Unit test for an example application that prints a data received
           message and requires no outside input.

    The data received message must be formatted as:
    ```
    Application received N bytes.
    ```
    """

    def __init__(self, application_name):
        super().__init__(application_name=application_name)
        self.data_received = False

    def check_pass_fail(self, exit_code):
        if exit_code == 0 and not self.data_received:
            print('No corrections data received.')
            return self.TEST_FAILED
        else:
            return super().check_pass_fail(exit_code)

    def on_stdout(self, line):
        if re.match(r'.*Application received \d+ bytes.', line):
            print('Corrections data detected.')
            self.data_received = True
            self.stop()
