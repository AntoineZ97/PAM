#!/bin/bash

function readUser {
echo "Please enter a userName"
read userName

if [ ${#userName} -lt 4 ]
then
	while [ ${#userName} -lt 4 ]
	do
		echo "Error: User name must have at least 4 characters"
		echo "Please enter a userName"
		read userName
	done
fi
}

function addUser {
echo "Setting userName ..."
useradd -m -s /bin/bash -G sudo $userName
res=$?
while [ $res -ne 0 ]
do
	echo "Error: Bad userName"
	readUser
	echo "Setting userName ..."
	useradd -m -s /bin/bash -G sudo $userName
	res=$?
done
echo "Success: User $userName has been added"
}

function setPassword {
echo "Setting password ..."
passwd $userName 
res=$?
while [ $res -ne 0 ]
do
	echo "Error: Bad password"
	echo "Setting password ..."
	passwd $userName 
	res=$?
done
echo "Success: You set the password for $userName"
}

function createContainer {
dd if=/dev/zero of=/home/$userName/encrypted bs=1M count=1024
cryptsetup luksFormat /home/$userName/encrypted
mkdir /home/$userName/secure_data-rw
cryptsetup luksOpen /home/$userName/encrypted crypt1
mount /dev/mapper/crypt1 /home/$userName/secure_data-rw/
mkfs.ext4 -j /dev/mapper/crypt1
mount /dev/mapper/crypt1 /home/$userName/secure_data-rw/
chown $userName -R /home/$userName/secure_data-rw/
chmod 700 /home/$userName/secure_data-rw
umount /dev/mapper/crypt1
cryptsetup luksClose crypt1
}

readUser
addUser
setPassword
createContainer

exit 0
