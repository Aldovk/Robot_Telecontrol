# README Configuracion Leader
~~Solo se realiza de esta manera en caso de que se prefiera mantener permanentemente esta configuracion.~~

Realizamos el siguiente cambio a las últimas lineas del script *~/.bashrc*, de tal manera que se inicialice de esta manera cada vez que se abra una nueva terminal. 
****
## Solo modificar si se desea un cambio permanente
**Inicialización ROS2 e inicialización del puente zenoh**

	echo "Hello from the shell :)"
	#ROS 2 Jazzy Setup
	source /opt/ros/jazzy/setup.bash
	export ROS_DOMAIN_ID=40
	export RMW_IMPLEMENTATION=rmw_zenoh_cpp

Solo se modifica la variable ROS_DOMAIN=40, en donde el ID de domain puede ser cualquiera entre 0 a 101. *Es preferible verificar que no existan IDs compartidos en la misma red.*
****
### Si los dispositivos se encuentran en diferentes redes Utilizar VPN Tailscale
Se instala usando apt y luego se inicia Tailscale siguiendo los siguientes comandos:

	curl -fsSL https://pkgs.tailscale.com/stable/ubuntu/focal.noarmor.gpg | sudo tee /usr/share/keyrings/tailscale-archive-keyring.gpg >/dev/null
	curl -fsSL https://pkgs.tailscale.com/stable/ubuntu/focal.tailscale-keyring.list | sudo tee /etc/apt/sources.list.d/tailscale.list
	sudo apt update
	sudo apt install tailscale   
	sudo systemctl start tailscaled
	sudo tailscale up
Puede ver el estado del vpn y del servicio utilizando:

	systemctl status tailscaled
También puede finalizar la sesión de tailscale usando:

	sudo systemctl stop tailscaled
	sudo tailscale down

_Se debe realizar un log in con una cuenta para ambos dispositivos y anotar las direcciones IP provistas por Tailscale para realizar el cambio a la dirección IP en el archivo zenoh_router.json5_
****
Para realizar la conexion al router, hay que iniciarlo como se procede a continuacion en una terminal:

	ZENOH_ROUTER_CONFIG_URI=$HOME/zenoh_router.json5 ros2 run rmw_zenoh_cpp rmw_zenohd

Esto le indica a la configuración del puente zenoh a utilizar el arhchivo .json5 en esta carpeta para su conexión. _Es necesario que se mantenga esta terminal ejecutandose durante toda la sesion de comunicación por ROS2._

Para cerrar el router zenoh utilice Ctrl + C en aquella terminal.