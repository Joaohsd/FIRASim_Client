# Evitar erro com o uso de video
xhost +local:robotbulls

## Buildando o docker
# docker build . -f Dockerfile -t nomedodocker
docker build . -f Dockerfile -t robotbulls
