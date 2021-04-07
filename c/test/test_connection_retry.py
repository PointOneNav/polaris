#!/usr/bin/env python3

import re

from test_application import TestApplication

class Test(TestApplication):
    def __init__(self):
        super().__init__(application_name='connection_retry')
        self.data_received = False

    def check_pass_fail(self, exit_code):
        if exit_code == 0 and not self.data_received:
            print('Test result: FAIL')
            return self.TEST_FAILED
        else:
            return super().check_pass_fail(exit_code)

    def on_stdout(self, line):
        if re.match(r'Application received \d+ bytes.', line):
            print('Corrections data detected.')
            self.data_received = True

test = Test()
test.parse_args()
test.run()
