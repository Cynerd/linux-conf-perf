## Configuration for ryuglab target
# This configuration overlaps default configuration if file .target
# contains "ryuglab"

kernel_arch = 'powerpc'
build_command = ['../targets/ryuglab/scripts/compile']
boot_command = ['targets/ryuglab/scripts/boot']

db_user = 'kocikare'
db_password = 'ohNg3Ien'

multithread = True


parse_kconfig_output = False
picosat_output = False
kernel_config_output = False
kernel_make_output = False
boot_output = False
parse_output = False


linux_sources = 'linux/'
linux_kconfig_head = linux_sources + 'Kconfig'
linux_dot_config = linux_sources + '.config'
linux_image = linux_sources + 'arch/' + kernel_arch + '/boot/uImage'
