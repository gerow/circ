project('circ', 'c', default_options : ['c_std=gnu17'], version : '0.1.0', license: 'Apache-2.0')

inc = include_directories('include')

subdir('include')
subdir('src')
subdir('test')

pkgconfig = import('pkgconfig')
pkgconfig.generate(libraries : lib,
                   version : '0.1',
                   name    : 'libcirc',
                   filebase : 'circ',
                   description : 'A virtual circular buffer library.')

