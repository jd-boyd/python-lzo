import tarfile
import os

os.chdir('C:\src')
tf=tarfile.open('C:\src\lzo-2.09.tar.gz', 'r:gz')
tf.extractall('.')
