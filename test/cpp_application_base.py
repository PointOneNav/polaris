import os
import sys

# Add the polaris/c/test/ directory to the search path, then import the C library's test base class.
repo_root_dir = os.path.normpath(os.path.join(os.path.dirname(__file__), '..'))
sys.path.append(os.path.join(repo_root_dir, 'c/test'))

from application_base import StandardApplication

class CppStandardApplication(StandardApplication):
    DEFAULT_ROOT_DIR = repo_root_dir
    DEFAULT_COMMAND = ['%(path)s', '--polaris_api_key=%(polaris_api_key)s', '--polaris_unique_id=%(unique_id)s']

    def __init__(self, application_name):
        super().__init__(application_name=application_name)
