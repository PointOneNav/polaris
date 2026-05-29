#!/bin/bash

set -e

echo "Testing simple_polaris_client..."
python3 test/test_simple_polaris_cpp_client.py $*

echo "Testing simple_polaris_client (no TLS)..."
python3 test/test_simple_polaris_cpp_client.py --polaris-use-tls=false $*
