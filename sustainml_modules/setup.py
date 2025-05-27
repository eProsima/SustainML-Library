"""Setup file to create sustainml_modules library."""
from setuptools import setup
import os

package_name = 'sustainml_modules'

description = 'SustainML Module Nodes Implementation'
long_description = description
with open(f'{os.path.dirname(os.path.realpath(__file__))}/README.md', 'r') as f:
    long_description = f.read()

file_packages = [
    package_name,
    package_name + '/sustainml-wp1',
    package_name + '/sustainml-wp2',
    package_name + '/sustainml-wp3',
    package_name + '/sustainml-wp5',
    package_name + '/sustainml-wp4',
]

setup(
    name=package_name,
    version='0.1.0',
    packages=file_packages,
    long_description=long_description,
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml', 'README.md']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='eprosima',
    maintainer_email='RaulSanchezMateos@eprosima.com',
    description=description,
    license='Apache License, Version 2.0',
    entry_points={},
    package_data=
    {
        "": ["*.ttl", "*/*"]
    }
)
