# youbot_lidar_nav

dependents: boost, cmake, make


## No EtherCAT connection... Excecute as root

The following error message is caused by the youbot_driver_ros_interface binary in catkin_ws/devel/lib/youbot_driver_ros_interface/ which doesn't have permissions to an ethernet device:

```
[ERROR] [1575985987.844141226]: No EtherCAT connection:
[FATAL] [1575985987.844197919]: No socket connection on enx00e04c2151d2
Excecute as root
```

See the original link: www.youbot-store.com/wiki/index.php/Execute_as_a_root. Usually, it's down, so check via the wayback machine.

In summary, to grant a binary permissions, run the following:

``` bash
$ sudo setcap cap_net_raw+ep <path_to_your_program_executable>/<name_of_your_program_executable>
$ sudo ldconfig <path_to_your_program_executable>
```
