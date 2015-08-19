## Configuration for ryuglab target
# This configuration overlaps default configuration if file .target
# contains "ryuglab"

kernel_arch = 'powerpc'
kernel_env = {'SRCARCH': kernel_arch, 'ARCH': kernel_arch, 'KERNELVERSION': kernel_arch}
build_command = ['make', 'uImage']
boot_command = ['targets/ryuglab/boot/boot']
parse_command = ['tests/cyclictest/parse']

db_user = 'kocikare'
db_password = 'ohNg3Ien'

multithread = True


parse_kconfig_output = False
picosat_output = False
kernel_config_output = False
kernel_make_output = False
boot_output = False
parse_output = False


linux_sources = 'targets/ryuglab/linux/'
linux_kconfig_head = linux_sources + 'Kconfig'
linux_dot_config = linux_sources + '.config'
linux_image = linux_sources + 'arch/' + kernel_arch + '/boot/uImage'
