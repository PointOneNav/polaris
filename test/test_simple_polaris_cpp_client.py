#!/usr/bin/env python3

from cpp_application_base import CppStandardApplication

class Test(CppStandardApplication):
    def __init__(self):
        super().__init__(application_name='simple_polaris_cpp_client')

test = Test()
test.parse_args()
test.run()
