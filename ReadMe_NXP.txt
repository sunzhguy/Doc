http://wiki.myzr.com.cn/index.php?title=MY-IMX6-EK140P_%E4%BD%BF%E7%94%A8%E5%8F%8A%E5%BC%80%E5%8F%91%E6%8C%87%E5%AF%BC


板子启动UBOOT信息【MYIMX6EK140P-6Y】
U-Boot 2016.03-svn350 (Jan 10 2019 - 17:44:43 +0800)

CPU:   Freescale i.MX6ULL rev1.0 696 MHz (running at 396 MHz)
CPU:   Industrial temperature grade (-40C to 105C) at 33C
Reset cause: POR
Board: MYIMX6EK140P-6Y
I2C:   ready
DRAM:  512 MiB
MMC:   FSL_SDHC: 0, FSL_SDHC: 1
Display: MY-WVGA (800x480)
Video: 800x480x24
In:    serial
Out:   serial
Err:   serial
Net:   FEC [PRIME]
Error: FEC address not set.


交叉工具链:
Z:\NXP_IM6ULL\03_toolchain
gcc-linaro-5.3

1 ./fsl-imx-x11-glibc-x86_64-meta-toolchain-qt5-cortexa7hf-neon-toolchain-4.1.15-2.1.0.sh
2       . /opt/fsl-imx-x11/4.1.15-2.1.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi 
3 source /opt/fsl-imx-x11/4.1.15-2.1.0/environment-setup-cortexa7hf-neon-poky-linux-gnueabi /执行修改编译器环境变量
u-boot编译:
1、make myimx6ek140p-6y-512m-emmc_defconfig 

2、make


内核编译:
1、make myimxa7_defconfig

2、make zImage
内核目标文件:
arch/arm/boot/zImage 即内核目标文件

3编译设备树目标文件:
make myimx6ek140p-6y-512m-emmc.dtb
out: arch/arm/boot/myimx6ek140p-6y-512m-emmc.dtb
4创建内核模块的保存目录
make modules


5安装内核模块到指定目录
make modules_install INSTALL_MOD_PATH=./mymodules

6打包内核模块文件


MY-IMX6-EK140           Linux-4.1.15
+++++++++++++++++++++++++++++++++++烧录:
core-image-base-myimx6a7.tar.bz2	24.3MB	MYIMX6A7 系列最小的（精简的） Linux 文件系统
(the simple Linux file system)
fsl-image-linux-myimx6a7.tar.bz2	120MB	MYIMX6A7 系列常规的 Linux 文件系统
(the common Linux file system)
fsl-image-qt5-myimx6a7.tar.bz2	    158MB	MYIMX6A7 系列Linux-QT5.6.1 文件系统
(Linux QT5 file system)



			UBoot 命令   
pirntenv  
pint 显示相关命令
setenv ipaddr

setenv  ipaddr     168.168.102.222
setenv  ethaddr    00:04:9f:04:d2:35
setenv  gatewayip  168.168.102.1
setenv  serverip     168.168.102.8

seveenv





设备树主文件：arch/arm/boot/dts/myimx6ek***-emmc.dts(或者是arch/arm/boot/dts/myimx6ek***-nand.dts)(“***”表示 140p-6g-128m，140p-6g-256m，140p-6g-512m，140p-6y-128m，140p-6y-256m , 140p-6y-512m,140-6g-128m，140-6g-256m，140-6g-512m，140-6y-128m，140-6y-256m 和 140-6y-512m)
设备树注册信息文件：arch/arm/boot/dts/myimx6ek***.dtsi (“***”表示 140，140p)
设备树管脚配置文件：arch/arm/boot/dts/myimx6ek***-iomuxc.dtsi (“***”表示 140，140p)
管脚功能预定义文件：arch/arm/boot/dts/imx6*-pinfunc.h(“*”表示 ul 或 ull





添加SPI 驱动到内核源码
diff --git a/arch/arm/boot/dts/myimx6ek140p-iomuxc.dtsi b/arch/arm/boot/dts/myimx6ek140p-iomuxc.dtsi
index c3296ab..966a2fa 100755
--- a/arch/arm/boot/dts/myimx6ek140p-iomuxc.dtsi
+++ b/arch/arm/boot/dts/myimx6ek140p-iomuxc.dtsi
@@ -24,7 +24,7 @@
         };
        pinctrl_myspi1_m4_irq: spi_irq_gpio1_14_grp {                                    //IOMUXC_SW_PAD_CTL_PAD_J
             fsl,pins = <                                                                //
-                MX6UL_PAD_JTAG_TCK__GPIO1_IO14                 0xf080          // M4--JCK SAI2
+                MX6UL_PAD_JTAG_TCK__GPIO1_IO14                 0xf080          // M14--JCK SAI2
             >;                                                                                             //
         };                                                                                                 //
         pinctrl_csi_i2c1: csi_i2c1_grp {
diff --git a/drivers/spi/Kconfig b/drivers/spi/Kconfig
index 72b0590..ce9225b 100644
--- a/drivers/spi/Kconfig
+++ b/drivers/spi/Kconfig
@@ -651,7 +651,11 @@ config SPI_SPIDEV

          Note that this application programming interface is EXPERIMENTAL
          and hence SUBJECT TO CHANGE WITHOUT NOTICE while it stabilizes.
-
+#config SPI_MYSPI_M4
+#      tristate "MYSPI M4 device driver support"
+#      help
+#        This supports SPI1 SPI2 for M4 SPI protocol drivers.
+source "drivers/spi/myspi_m4/Kconfig"
diff --git a/drivers/spi/Makefile b/drivers/spi/Makefile
index d8cbf65..ed8fbf7 100644
--- a/drivers/spi/Makefile
+++ b/drivers/spi/Makefile
@@ -8,7 +8,9 @@ ccflags-$(CONFIG_SPI_DEBUG) := -DDEBUG
 # config declarations into driver model code
 obj-$(CONFIG_SPI_MASTER)               += spi.o
 obj-$(CONFIG_SPI_SPIDEV)               += spidev.o
+#obj-$(CONFIG_SPI_MYSPI_M4)            += spim4.o
+#spim4-objs                              :=spi_m4.o lkj_pro.o
+obj-$(CONFIG_SPI_MYSPI_M4)             += myspi_m4/















