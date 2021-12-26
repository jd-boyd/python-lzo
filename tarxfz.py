import tarfile
import os

os.chdir('C:\src')
tf=tarfile.open('C:\src\lzo-2.10.tar.gz', 'r:gz')
tf.extractall('.')
