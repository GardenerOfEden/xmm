# System imports
from distutils.core import *
from distutils      import sysconfig

# Third-party modules - we depend on numpy for everything
import numpy

# Obtain the numpy include directory.
numpy_include = numpy.get_include()

src_dir = '../src/models/'
rtml_dir = '../src/base/'

# mhmm_lib extension module
mhmm_module = Extension('_mhmm',
                        define_macros=[('USE_PTHREAD', None)],
                        extra_link_args = ["-lpthread"],
                        sources=['mhmm_wrap.cxx', rtml_dir + 'json_utilities.cpp',
                                    rtml_dir + 'label.cpp', rtml_dir + 'phrase.cpp',
                                    rtml_dir + 'training_set.cpp',
                                    rtml_dir + 'probabilistic_model.cpp', rtml_dir + 'gaussian_distribution.cpp',
                                    src_dir + 'gmm.cpp', src_dir + 'gmm_group.cpp',
                                    src_dir + 'hmm.cpp', src_dir + 'hierarchical_hmm.cpp', 
                                    src_dir + 'kmeans.cpp'],
                        include_dirs = [numpy_include, '../components/libjson/src/', src_dir, rtml_dir],
                        library_dirs = ['../components/libjson/built-lib'],
                        libraries = ['json']
                        )
# mhmm_lib setup
setup (name        = 'mhmm',
       version     = '0.2',
       author      = "Jules Francoise <jules.francoise@ircam.fr>",
       description = """Multimodal Hidden Markov Models Library""",
       ext_modules = [mhmm_module],
       py_modules  = ["mhmm"],
       )