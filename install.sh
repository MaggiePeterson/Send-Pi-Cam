#!/bin/sh
sudo raspi-config nonint do_boot_behaviour B2
echo "#!/bin/sh
cd /home/pi/send-Pi-Cam
su pi -c 'sudo ./opencv_cam_server'" > startVisionServer.sh
chmod a+x startVisionServer.sh

#set up the service file --  service tells linux to run this
echo "[Unit]
Description= Send Pi Cam
After=multi-user.target
[Service]
Type=idle
ExecStart=/home/pi/send-Pi-Cam/startVisionServer.sh
RemainAfterExit=no
Restart=on-failure
RestartSec=5s
[Install]
WantedBy=multi-user.target" | sudo tee /lib/systemd/system/sendPiCam.service

sudo chmod 644 /lib/systemd/system/sendPiCam.service #changes permission of service to run
#enable service
sudo systemctl daemon-reload
sudo systemctl enable sendPiCam.service

#1. chmod +x /path/to/install.sh
#2. sudo ./install.sh
#3. reboot pi, then
#4. sudo systemctl status sendPiCam
