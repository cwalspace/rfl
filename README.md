# roll for life readme

## build
```
cmake CMakeLists.txt
make
```

## usage

1. put websites you want to promote in refs.txt
```
place http:// or https:// at the start of your domains
```
2. add domains to hosts.txt that output referral links on their website
```
do not use http:// or https:// here
```
3. rfl \<max threads\> \<hosts file\> <refs file\>

## example

>./rfl 30 hosts.txt refs.txt

## more info

- you can place user agents in agents.txt
- format your lists like this: 
```
line1
line2
``` 
- right now everything is random, i will add new options in the future
- you can check out free host list (periodically updated) at http://cwal.space/runforlinks