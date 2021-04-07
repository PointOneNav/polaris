#!/usr/bin/env python3

from application_base import StandardApplication

class Test(StandardApplication):
    def __init__(self):
        super().__init__(application_name='connection_retry')

test = Test()
test.parse_args()
test.run()
