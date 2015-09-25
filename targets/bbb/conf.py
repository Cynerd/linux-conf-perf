## Configuration for bbb target
# This configuration overlaps default configuration if file .target
# contains "bbb"

kernel_arch = 'arm'
kernel_env = {'SRCARCH': kernel_arch, 'ARCH': kernel_arch, 'KERNELVERSION': kernel_arch}
build_command = ['make']
boot_command = ['targets/bbb/boot/boot']
parse_command = ['tests/cyclictest/parse']

db_user = 'kocikare'
db_password = 'ohNg3Ien'

multithread = False


parse_kconfig_output = False
picosat_output = False
kernel_config_output = False
kernel_make_output = False
boot_output = False
parse_output = False


linux_sources = 'targets/bbb/linux/'
linux_build_folder = 'targets/bbb/build/'
linux_image = linux_build_folder + 'arch/' + kernel_arch + '/boot/uImage'

test_config_cyclictest = 'targets/bbb/cyclictest_config'
