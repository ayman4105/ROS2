from setuptools import find_packages, setup

package_name = 'obstacle_detection_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/obstacle_detection_pkg/launch', ['launch/obstacle.launch.py']),            
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ayman',
    maintainer_email='abohamedayman22@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'obsDetect = obstacle_detection_pkg.obsDetect:main',
            'safetyNode = obstacle_detection_pkg.SafetyNode:main',    
        ],
    },
)
