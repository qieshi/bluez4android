# bluez4android
最近在调试安卓的蓝牙硬件，需要用到较为底层的bluez工具：hciconfig、hcitool、hcidump、hciattach、btmon。<br>
然而在网上看了一下，bluez已经不支持安卓了，于是自己找来源码包，将bluez成功移植到目前的安卓板子上，都可以正常使用。
## 使用方法
将本项目直接copy到android项目中的externel目录下，然后在安卓项目根目录下<br>
source build/envsetup.sh <br>
lunch xxx <br>
再cd externel/本项目目录/bluez <br>
最后再mm一下即可。
## 注
移植所用的原始tar包在origin_tar目录下，诸位可以根据自己的情况对其进行个性化移植
