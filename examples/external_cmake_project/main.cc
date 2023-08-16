/**************************************************************************/ /**
 * @brief Example external cmake project.
 *
 * Copyright (c) Point One Navigation - All Rights Reserved
 ******************************************************************************/

#include <point_one/polaris/portability.h>
#include <point_one/polaris/polaris_client.h>

using namespace point_one::polaris;

int main(int argc, const char* argv[]) {
  P1_printf("Hello, world!\n");
  P1_printf("sizeof(PolarisClient): %zu B\n", sizeof(PolarisClient));
  return 0;
}
