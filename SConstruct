import os, imp
sconsutils = imp.load_source('sconsutils', os.path.join(os.pardir, os.pardir, "build", "sconsutils.py"))

env = sconsutils.getEnv()
env.InVariantDir(env['oDir'], ".", lambda env: env.LibAndApp('bitset', 0, -1, (
  ('core', 0, 0),
)))
