###
 # @Descripttion: 
 # @vesion: 
 # @Author: sunzhguy
 # @Date: 2020-04-27 14:30:55
 # @LastEditor: sunzhguy
 # @LastEditTime: 2020-04-27 14:45:09
 ###
#!/bin/bash
file=/etc/ifconfig-settings
val="";
echo $file
if [ -r $file ] ;then
echo "the file is be read"
else
echo "the file no be read"
chmod 666 $file
fi
get_ip(){
   echo $1
   val=$( ifconfig $1 | awk -F'[ :]+' '/inet\ addr/{print $4}')
}
#获取子网掩码
get_mask(){
   val=$(ifconfig $1 | awk -F: '/Mask/{print $NF}')
}
#获取mac地址
get_mac(){
   val=$( ifconfig $1 | awk '/HWaddr/{print $NF}')
}
#获取广播地址
get_bcast(){
    val=$(ifconfig $1 | awk -F'[ :]+' '/Bcast/{print $6}')
}
#获取网管地址
get_route(){
    val=$(route -n | awk '/UG/{print $2}')   
}
function getParam()
{
 val=$(cat $file|grep  "$1="|awk -F '=' '{print $2}')
}
function fix_mac()
{
  mac=$(ifconfig eth0|grep eth0|awk '{print $5}')
  #echo $mac
 sed -i  "s/\(MAC=\).*/\1${mac}/" $file 
  #sed -i "s/MAC/MAC=$mac/" $file 
}
ifconfig eth0 down
getParam "MAC" 
if  [ -z $val ];then #if the mac is empty to add mac  not judge mac is useful
  fix_mac
else
  echo "ifconfig"
  echo $val
  ifconfig eth0 hw ether $val
fi
getParam "DHCP"
if [ $val -eq 1 ]; then
echo "udhcpc"
 udhcpc /etc/udhcpd.conf
 if [ $? -eq 0 ]; then
   get_ip eth0
   #echo $val
  sed -i  "s/\(IP=\).*/\1${val}/" $file 
   get_mask eth0 
  sed -i  "s/\(NETMASK=\).*/\1${val}/" $file 
   #echo $val
   get_route eth0
  sed -i  "s/\(GATEWAY=\).*/\1${val}/" $file 
  sed -i  "s/\(DNS=\).*/\1${val}/" $file 
  sed -i  "s/\(DHCP=\).*/\10/" $file 
   #echo $val
  #ip=ifconfig eth0 |grep 'inet addr:'|cut -d: -f2 |awk '{print $1}'
  #echo $ip
   
  fi
else
getParam "IP" 
 ip=$val
getParam "NETMASK" 
 netmask=$val
getParam "GATEWAY"
 gateway=$val
getParam  "DNS"
 dns=$val
echo $ip $gateway $netmask $dns
ifconfig eth0 $ip netmask $netmask
route add default gw $gateway
echo nameserver $dns > /etc/resolv.conf
ifconfig eth0 up
fi


#ifconfig-settings
#DHCP=0
#MAC=
#IP=192.168.10.10
#NETMASK=255.255.255.0
#GATEWAY=192.168.10.1
#DNS=192.168.10.1
#

