#!/bin/bash

set -e

echo "Testing simple_polaris_client..."
python3 test/test_simple_polaris_client.py $*

echo "Testing connection_retry..."
python3 test/test_connection_retry.py $*
