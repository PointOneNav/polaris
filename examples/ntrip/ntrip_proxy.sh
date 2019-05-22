#!/bin/bash

bazel-run.sh -c opt :ntrip_example -- --polaris_api_key=${MY_POLARIS_APP_KEY} --logtostderr 0.0.0.0 2101 .