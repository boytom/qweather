获取省级代码：http://www.weather.com.cn/data/list3/city.xml?level=1
获取城市代码(比如安徽是22)：http://www.weather.com.cn/data/list3/city22.xml?level=2
获取区域代码（比如安庆是2206）：http://www.weather.com.cn/data/list3/city2206.xml?level=3
获取到安徽省安庆市望江县的代码是220607
然后去加上中国代码请求URL：http://m.weather.com.cn/data/101220607.html
就可以获取当地天气。

该api获取天气的接口如下：
http://www.weather.com.cn/data/sk/101281601.html 
http://www.weather.com.cn/data/cityinfo/101281601.html 
http://m.weather.com.cn/data/101281601.html  
3种返回的数据稍微有些不同，至于使用哪一种，看个人喜欢吧～～
值得说明的是101281601是城市在中央气象台数据库中城市码，获取的方法有些麻烦，有以下几个步骤：
1. 通过 http://m.weather.com.cn/data5/city.xml 获取省份列表；
2. 通过 http://m.weather.com.cn/data5/city28.xml 获取该省份的城市列表，其中28为广东省代号，1中所返回的；
3. 通过 http://m.weather.com.cn/data5/city2816.xml 获取该城市的区列表，其中2816为东莞市代号，2中所返回的；
4. 通过 http://m.weather.com.cn/data5/city281601.xml 获取最后的城市码，其中281601为3中返回；
5. 通过 api获取天气信息；
