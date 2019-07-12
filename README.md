waxe
====

Haxe + wxWidgets


Rebuilding
----------

```
git clone --recurse https://github.com/nmehost/waxe.git
cd waxe
haxelib dev waxe .
cd project
haxelib run hxcpp Build.xml -DHXCPP_M64
```

If you want to do 32-bit neko:
```
haxelib run hxcpp Build.xml
```


Testing
----------

```
cd samples/00-Minimal
haxe compile-cpp.hxml
./bin/cpp/Main.exe
```

neko testing:

```
cd samples/01-Simple
haxe compile-neko.hxml
neko Sample.n
```

