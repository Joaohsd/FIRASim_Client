DEFAULT_DOCKER_IMAGE="robotbulls"	#	(robotbulls)
DEFAULT_CONTAINER_NAME="robotbulls_container"			#	(robotbulls_container)

WORK_DIR=`pwd`
CONTAINER_WORK_DIR=$WORK_DIR

CONTAINER_NAME=$DEFAULT_CONTAINER_NAME
DOCKER_IMAGE=$DEFAULT_DOCKER_IMAGE

# Executando o docker
sudo docker run -it \
            --user=$(id -u) \
            --env="DISPLAY" \
            --env="QT_X11_NO_MITSHM=1" \
            --name=$CONTAINER_NAME \
            --memory=1024g \
            --oom-kill-disable \
            --ipc="host" \
            --volume="/dev:/dev" \
            --privileged \
            --net=host \
            --workdir="${CONTAINER_WORK_DIR}" \
            --volume="${WORK_DIR}:${CONTAINER_WORK_DIR}" \
            --volume="/etc/group:/etc/group:ro" \
            --volume="/etc/passwd:/etc/passwd:ro" \
            --volume="/etc/shadow:/etc/shadow:ro" \
            --volume="/etc/sudoers.d:/etc/sudoers.d:ro" \
            --volume="/tmp/.X11-unix:/tmp/.X11-unix:rw" \
            $DOCKER_IMAGE

sudo docker container rm $CONTAINER_NAME -f
