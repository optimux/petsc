import config.base

import os

class Configure(config.base.Configure):
  def __init__(self, framework):
    config.base.Configure.__init__(self, framework)
    self.headerPrefix = ''
    self.substPrefix  = ''
    self.compilers    = self.framework.require('config.compilers', self)
    self.libraries    = self.framework.require('config.libraries', self)
    self.include      = None
    self.lib          = None
    return

  def __str__(self):
    return ''

  def getIncludeArgument(self, include):
    '''Return the proper include line argument for the given filename
       - If the path is empty, return it unchanged
       - If starts with - then return unchanged
       - Otherwise return -I<include>'''
    if not include:
      return ''
    if include[0] == '-':
      return include
    return '-I'+include

  def checkInclude(self, includeDir):
    '''Check that Python.h is present'''
    oldFlags = self.compilers.CPPFLAGS
    self.compilers.CPPFLAGS += ' '+' '.join([self.getIncludeArgument(inc) for inc in includeDir])
    found = self.checkPreprocess('#include <Python.h>\n')
    self.compilers.CPPFLAGS = oldFlags
    return found

  def checkPythonLink(self, includes, body, cleanup = 1, codeBegin = None, codeEnd = None, shared = 0):
    '''Analogous to checkLink(), but the Python includes and libraries are automatically provided'''
    success  = 0
    oldFlags = self.compilers.CPPFLAGS
    oldLibs  = self.framework.argDB['LIBS']
    self.compilers.CPPFLAGS += ' '+' '.join([self.getIncludeArgument(inc) for inc in self.include])
    self.framework.argDB['LIBS'] = ' '.join([self.libraries.getLibArgument(lib) for lib in self.lib])+' '+self.framework.argDB['LIBS']
    if self.checkLink(includes, body, cleanup, codeBegin, codeEnd, shared):
      success = 1
    self.compilers.CPPFLAGS = oldFlags
    self.framework.argDB['LIBS'] = oldLibs
    return success

  def configurePythonLibraries(self):
    import distutils.sysconfig
    import os

    inc = [distutils.sysconfig.get_python_inc()]
    if not self.checkInclude(inc):
      raise RuntimeError('Unable to locate Python headers')
    self.include = inc

    lib = os.path.join(distutils.sysconfig.get_config_var('LIBDIR'), distutils.sysconfig.get_config_var('LDLIBRARY'))
    if not os.path.isfile(lib):
      lib = os.path.join(distutils.sysconfig.get_config_var('LIBPL'), distutils.sysconfig.get_config_var('LDLIBRARY'))
      if not os.path.isfile(lib):
        lib = os.path.join('/System','Library','Frameworks',distutils.sysconfig.get_config_var('LDLIBRARY'))
        if not os.path.isfile(lib):
          lib = os.path.join('/Library','Frameworks',distutils.sysconfig.get_config_var('LDLIBRARY'))
          if not os.path.isfile(lib):
            raise RuntimeError("Cannot locate Python dynamic libraries");
        
    ext = distutils.sysconfig.get_config_var('SO')
    if os.path.isfile(lib.split(ext)[0]+ext): lib = lib.split(ext)[0]+ext

    self.lib = [lib]
    if distutils.sysconfig.get_config_var('LIBS'):
      flags = distutils.sysconfig.get_config_var('LIBS')
      if distutils.sysconfig.get_config_var('LDFLAGS'):
        flags = distutils.sysconfig.get_config_var('LDFLAGS')+' '+flags
      self.lib.extend(self.splitLibs(flags))

    if not self.libraries.checkShared('#include <Python.h>\n', 'Py_Initialize', 'Py_IsInitialized', 'Py_Finalize', checkLink = self.checkPythonLink, libraries = self.lib, initArgs = '', noCheckArg = 1):
      raise RuntimeError('Python library must be shared')
    return

  def setOutput(self):
    '''Add defines and substitutions
       - PYTHON_INCLUDE and PYTHON_LIB are command line arguments for the compile and link
       - PYTHON_INCLUDE_DIR is the directory containing mpi.h
       - PYTHON_LIBRARY is the list of Python libraries'''
    if self.include:
      self.addSubstitution('PYTHON_INCLUDE',     ' '.join(['-I'+inc for inc in self.include]))
      self.addSubstitution('PYTHON_INCLUDE_DIR', self.include[0])
    else:
      self.addSubstitution('PYTHON_INCLUDE',     '')
      self.addSubstitution('PYTHON_INCLUDE_DIR', '')
    if self.lib:
      self.addSubstitution('PYTHON_LIB',     ' '.join(map(self.libraries.getLibArgument, self.lib)))
      self.addSubstitution('PYTHON_LIBRARY', self.lib)
    else:
      self.addSubstitution('PYTHON_LIB',     '')
      self.addSubstitution('PYTHON_LIBRARY', '')
    return

  def configure(self):
    self.executeTest(self.configurePythonLibraries)
    self.setOutput()
    return
