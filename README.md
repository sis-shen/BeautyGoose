- [2.x版本更新信息](#2x版本更新信息)
  - [v2.0重大版本更新](#v20重大版本更新)
  - [v2.1参数更新](#v21参数更新)
  - [v2.2日志输出更新](#v22日志输出更新)
  - [v2.3数据库守护线程更新](#v23数据库守护线程更新)
  - [v2.4 反向代理更新](#v24-反向代理更新)
  - [v2.5 Redis缓冲层更新](#v25-redis缓冲层更新)
  - [v2.6 网络框架替换更新](#v26-网络框架替换更新)
- [BeautyGoose](#beautygoose)
- [项目介绍](#项目介绍)
  - [运行环境](#运行环境)
  - [需求分析](#需求分析)
    - [用户需求](#用户需求)
  - [角色抽象归纳](#角色抽象归纳)
    - [用例图](#用例图)
  - [功能需求](#功能需求)
  - [与其它系统的接口](#与其它系统的接口)
    - [线上支付服务](#线上支付服务)
    - [数据库服务](#数据库服务)
    - [Redis服务](#redis服务)
  - [软件需求规格](#软件需求规格)
    - [系统架构](#系统架构)
      - [系统结构图](#系统结构图)
    - [系统层次结构](#系统层次结构)
  - [软件模块设计](#软件模块设计)
    - [账户管理模块](#账户管理模块)
    - [第三方支付模块](#第三方支付模块)
    - [消费者模块](#消费者模块)
    - [商家模块](#商家模块)
    - [管理员模块](#管理员模块)
  - [数据库设计](#数据库设计)
    - [数据库文件和连接账户](#数据库文件和连接账户)
      - [建数据库](#建数据库)
      - [创建专门的连接用用户](#创建专门的连接用用户)
    - [数据表设计](#数据表设计)
      - [建立账户表](#建立账户表)
      - [建立菜品表](#建立菜品表)
      - [建立订单表](#建立订单表)
      - [建立历史订单表](#建立历史订单表)
      - [建立orderDish表](#建立orderdish表)
      - [建立historyDish表](#建立historydish表)
- [项目设计](#项目设计)
- [服务端详细设计](#服务端详细设计)
  - [版本信息](#版本信息)
  - [HTTP请求路径约定](#http请求路径约定)
    - [账户相关 Account API](#账户相关-account-api)
    - [消费者相关 Consumer API](#消费者相关-consumer-api)
    - [商家相关 Merchant API](#商家相关-merchant-api)
  - [管理员相关 Admin API](#管理员相关-admin-api)
- [客户端详细设计](#客户端详细设计)
  - [版本信息](#版本信息-1)
  - [用户界面设计](#用户界面设计)
    - [界面流转流程图](#界面流转流程图)
    - [注册界面](#注册界面)
    - [用户名登录界面](#用户名登录界面)
    - [手机号登录界面](#手机号登录界面)
    - [账户优惠等级升级界面](#账户优惠等级升级界面)
    - [消费者菜品列表界面](#消费者菜品列表界面)
    - [消费者菜品详情窗口](#消费者菜品详情窗口)
    - [消费者购物车列表界面](#消费者购物车列表界面)
    - [消费者查看订单列表界面](#消费者查看订单列表界面)
    - [消费者查看订单详情界面](#消费者查看订单详情界面)
    - [商家菜品列表界面](#商家菜品列表界面)
    - [商家菜品注册界面](#商家菜品注册界面)
    - [商家菜品详情窗口](#商家菜品详情窗口)
    - [商家菜品编辑窗口](#商家菜品编辑窗口)
    - [商家订单列表界面](#商家订单列表界面)
    - [商家订单详情窗口](#商家订单详情窗口)
    - [管理员日志查看界面](#管理员日志查看界面)
    - [第三方支付窗口](#第三方支付窗口)
  - [类图设计](#类图设计)


# 2.x版本更新信息
## v2.0重大版本更新
+ 服务端完成开发与部署环境迁移
    + 从Windows11 平台转移到了 Ubuntu22.04云服务器环境
+ Docker容器与镜像管理技术引入，使用docker-compose.yml实现一键构建和部署

## v2.1参数更新
+ 删除了json配置文件，添加了.conf配置文件
+ 使用`gflags`库用于命令行参数的声明和解析
+ 使用.conf配置文件控制参数

## v2.2日志输出更新
+ 删除了原本简陋的日志输出方式
+ 二次封装了spdlog，使用统一的全局日志器和日志格式
+ 按照不同的等级在程序的各个关键点配置了日志输出。

## v2.3数据库守护线程更新
+ 为`DatabaseClient`配置了一个负责重连的异步守护线程
+ 完成了`DatabaseClient`和`HTTPServer`的解耦合
+ 为每一个数据库客户端接口的异常处理处加入了被动检测连接状态
+ 异步线程循环中加入了计数器，能够定时自动触发连接状态检测

## v2.4 反向代理更新
+ 架构上添加代理层，升级为应用服务集群架构
+ 容器编排上，后端服务程序数量提升为两个
+ 采用反向代理负载均衡技术，后端服务器均衡承载负载

## v2.5 Redis缓冲层更新
+ 在容器编排中引入Redis 7.2.4
+ 将Redis服务作为MySQL缓冲层
+ 修复客户端部分bug，并使用Release模式重新编译

## v2.6 网络框架替换更新
+ 移除cpp-httplib网络框架
+ 增加drogon网络框架
+ 将网络API的业务代码解耦合分离到不同源文件中
+ 将服务端server的项目构建方式改为使用CMake

# BeautyGoose
一款基于QT框架，采用CS架构的外卖模拟平台

[戳我去博客网站](https://www.supdriver.top/2025/02/23/BeautyGoose-collection2/)

# 项目介绍
为客户提供一个C/S架构的外卖平台，使用Qt框架和cpp-httplib 等模块实现其业务功能。能够服务消费者，商家和管理员的使用需求，目前支持的客户端平台为Windows平台。服务端平台只需支持Docker即可

使用的技术点有:

+ QT界面框架
+ Json序列化与反序列化
+ drogon现代化的网络库
+ MySQL connector/c++
+ Redis 数据库缓冲层
+ Docker容器编排

## 运行环境
*当前版本v2.6*

客户端: windows 11

服务端: 阿里云ECS云服务器 ubuntu 22.04 2核2G
## 需求分析

### 用户需求
当前目标实现的用户需求有

1. 用户注册、登录
2. 商家添加产品
3. 商家查看和修改产品
4. 商家查看和处理订单
5. 消费者查看和添加购物车
6. 消费者查看和结算订单
7. 消费者优惠等级变化
8. 管理员查看销售日志

## 角色抽象归纳
根据需求的功能点，可以将系统的参与者抽象成两层三类。

+ `用户`: 所有具体参与者的总称，更高一级的抽象
+ `商家`：提供菜品和处理订单的一类人
+ `消费者`：点餐和请求订单的一类人
+ `管理员`：有权限查看销售日志的一类人

### 用例图
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501171350578.webp)


## 功能需求
根据用户的需求，我们提出更具体，更偏向实现的功能需求

*阅读提示：点击右侧目录可以跳转*

| 功能点 | 描述 |
| --- | ---- | 
|用户注册功能 | 用户可以使用唯一的***账号名和手机号**和进行账号注册| 
|用户登录功能 | 用户可以使用**账号名或手机号**进行账号登录 |
|商家菜品注册功能 |商家可以使用菜品注册功能来注册新的菜品|
|商家菜品查看功能 | 商家可以看到自己的所有菜品列表，也能够看到某个菜品的详情 |
|商家菜品修改功能 |商家可以修改菜品的信息，也可以删除菜品|
|商家订单查看功能 |商家能够看到自己的订单列表，也可以看到某个订单的详情|
| 商家订单处理功能 | 商家能够选择接单或拒单 |
| 消费者查看菜品功能 | 消费者可以看到所有菜品列表，也可以看到某一个菜品的详情 |
| 消费者购物车添加功能 | 消费者能够将菜品添加到对应商家的购物车中 |
|消费者查看购物车功能 | 消费者能够看到所有商家的购物车列表 |
| 消费者订单生成功能 |  消费者能够将指定的购物车生成为订单 | 
| 消费者第三方支付功能 | 消费者能够使用第三方支付订单 | 
| 消费者订单支付功能 | 消费者能够支付订单并支付完成的订单被商家看到 |
| 消费者提升优惠等级功能 | 消费者能够看到自己的优惠等级，并通过购买VIP的方式提升优惠等级 |
| 管理员日志查看功能 | 使用管理员账户登录后，可以看到所有的销售日志 | 
## 与其它系统的接口

### 线上支付服务
因为非实际生产的项目，因此不接入第三方支付接口

### 数据库服务
本平台使用`MySQL 8.0`提供的数据库管理服务,使用相关的API构建自定义客户端

### Redis服务
本平台使用`Redis 7.2.4`提供的内存级数据库服务，使用相关的API构建自定义客户端

## 软件需求规格

### 系统架构

#### 系统结构图
+ 组件图
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501171433499.webp)

+ 应用数据架构图
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501191456115.webp)

本版本采用在开发机上部署服务端，云服务器上部署数据库，并使用云服务器提供内网穿透服务的方式为客户端提供服务
### 系统层次结构
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501191347270.webp)

## 软件模块设计

### 账户管理模块
1. 账户注册功能         accountRegister
2. 账户用户名登录功能   accountLoginByName
3. 账户手机号登录功能   accountLoginByPhone
4. 账户优惠等级升级功能 accountUpdateLevel

### 第三方支付模块
1. 第三方支付功能       thirdPay

### 消费者模块
1. 消费者查看菜品列表功能   consumerToDishList
2. 消费者查看菜品详情功能   consumerToDishDetail
3. 消费者购物车添加功能     consumerCartAdd
4. 消费者购物车移除功能     consumerCartPop
5. 消费者购物车列表查看功能 consumerToCartList
6. 消费者订单生成功能       consumerOrderGenerate
7. 消费者订单详情查看功能   consumerOrderDetail
8. 消费者订单支付功能能     consumerOrderPay
9. 消费者订单取消功能       consumerOrderCancel

### 商家模块
1. 商家菜品列表查看功能     merchantToDishList
2. 商家菜品注册功能         merchantDishRegister
3. 商家菜品详情查看功能     merchantDishDetail
4. 商家菜品修改功能         merchantDishEdit
5. 商家菜品删除功能         merchantDishDel 
6. 商家订单列表查看功能     merchantToOrderList
7. 商家订单详情查看功能     merchantOrderDetail
8. 商家订单接单功能         merchantOrderAccept
9. 商家订单拒单功能         merchantOrderReject

### 管理员模块
1. 日志查询功能             adminHistoryLog

## 数据库设计
本项目采用Docker镜像`mysql:8.0`

### 数据库文件和连接账户
#### 建数据库
```sql
create database BeautyGoose;
use BeautyGoose;
```

#### 创建专门的连接用用户
```sql
set global validate_password_policy = 'LOW';
create user 'btyGooseUser'@'%' identified by 'BueatyGoose';
grant all on BeautyGoose.* to 'btyGooseUser'@'%';
flush privileges;
```
### 数据表设计
![account](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192017506.webp)

![dish](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192024255.webp)

![order](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192026572.webp)

![history](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192025124.webp)

![orderDish](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192027578.webp)

![history](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192027578.webp)

#### 建立账户表
因为本项目的重点是功能实现，数据安全目前暂不考虑，所以所有数据采用明文存储的方式

```sql
create table account(
    uuid varchar(13) unique,
    phone varchar(12) unique,
    name varchar(10) not null,
    password varchar(64) not null,
    nickname varchar(32),
    level tinyint not null default 0,
    icon_data blob,
    type tinyint not null default 0,
    balance int unsigned default 0
);

```

#### 建立菜品表

```sql
create table dishes(
    uuid varchar(13) unique,
    merchant_id varchar(13) not null,
    merchant_name varchar(10) not null,
    name varchar(10) not null,
    icon_path varchar(150),
    description text,
    base_price decimal(6,2),
    price_factor float(4,2) unsigned,
    is_delete boolean not null default 1
);
```

#### 建立订单表

```sql
create table orders(
    time varchar(32) not null, 
    uuid varchar(13) unique,
    merchant_id varchar(13) not null,
    merchant_name varchar(10) not null,
    consumer_id varchar(13) not null,
    consumer_name varchar(10) not null,
    level tinyint not null default 0,
    pay decimal(10,2) not null default 0,
    status tinyint not null default 0,
    sum int not null
);
```

#### 建立历史订单表

```sql
create table history(
    time varchar(32) not null, 
    uuid varchar(13) unique,
    merchant_id varchar(13) not null,
    merchant_name varchar(10) not null,
    consumer_id varchar(13) not null,
    consumer_name varchar(10) not null,
    level tinyint not null default 0,
    pay decimal(10,2) not null default 0,
    status tinyint not null default 0,
    sum int not null
);
```

#### 建立orderDish表

```sql
create table orderDish(
    order_id varchar(13) not null,
    dish_id varchar(13) not null,
    merchant_id varchar(13) not null,
    name varchar(10) not null,
    dish_price decimal(6,2),
    count int not null default 0
);
```

#### 建立historyDish表

```sql
create table historyDish(
    order_id varchar(13) not null,
    dish_id varchar(13) not null,
    merchant_id varchar(13) not null,
    name varchar(10) not null,
    dish_price decimal(6,2),
    count int not null default 0
);
```

# 项目设计

# 服务端详细设计
## 版本信息
**当前版本**: v2.6网络框架替换更新

当前版本的服务器使用了如下技术栈:

+ `jsoncpp`: 本版本对报文的序列化与反序列化技术依然使用了`Json`格式
+ `drogon`:一个开源的现代网络库，提供高版本C++下高效的网络服务
+ `MySQL Connector/C++`: MySQL官方提供的C++连接数据库的API
+ `Redis 键值对存储`: 开源，高效的中间层缓冲服务
+ `Docker 容器编排`: 将所有的服务使用容器化的方式组织管理

## HTTP请求路径约定
客户端与服务端的交互对具体函数的调用，取决于`HTTP 请求路径`的约定，所以要提前约定好

### 账户相关 Account API
| 路径 | 业务 |
|--- | --- | 
| `/account/register` | 账户**注册** |
| `/account/login/username` | 账户使用**用户名登录** | 
| `/account/login/phone` | 账户使用**手机号登录** | 
| `/account/update/level` | 消费者账户**优惠等级更新** | 
| `/account/update/nickname` | 账号**更新昵称** |

### 消费者相关 Consumer API
| 路径 | 业务 |
|--- | --- | 
| `/consumer/order/detail/dishlist` | 消费者获取**订单**详情内的**菜品列表** |
| `/consumer/dish/list` | 消费者**菜品列表** |
| `/consumer/dish/dishInfo` | 消费者获取**菜品详情** | 
| `/consumer/order/generate` | 消费者获**取订单生成** |
| `/consumer/order/list` | 消费者**获取订单列表** |
| `/consumer/order/pay/confirm` | 消费者**完成订单支付** | 
| `/consumer/order/cancel` | 消费者**请求取消订单** |

### 商家相关 Merchant API
| 路径 | 业务 |
| --- | --- |
| `/merchant/dish/list` | 商家获取**菜品列表** | 
| `/merchant/dish/register` | 商家请求**注册菜品** | 
| `/merchant/dish/info` | 商家获取**菜品详情** | 
| `/merchant/dish/update` | 商家请求**更新菜品** |
| `/merchant/dish/del` | 商家请求**删除菜品** |
| `/merchant/order/list` | 商家请求**订单列表** |
| `/merchant/order/detail/dishlist` | 商家请求**订单详情的菜品列表** | 
| `/merchant/order/accept` | 商家**接单** | 
| `/merchant/order/reject` | 商家**拒单** |

## 管理员相关 Admin API
| 路径 | 业务 |
| --- | --- |
| `/admin/order/list` | 管理员获取订单列表 |

# 客户端详细设计

## 版本信息
**当前版本**: v1.0正式版

当前版本的界面属于类似于毛坯房的只有UI结构和功能而没有美化，且所有的图片显示均尚未处理,均使用了QLabel进行占位

## 用户界面设计
在使用代码工具编写界面前，可以先使用作图工具设计（“画”）一个UI界面出来，这样子有助于将UI设计与代码开发有**一定程度的解耦合**。降低开发难度

### 界面流转流程图
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501201347364.png)

### 注册界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192048136.png)

1.	注册按钮： 点击请求注册
2.	转到用户名登录按钮： 点击转到用户名登录界面
3.	转到手机号登录按钮：点击转到手机号登录

### 用户名登录界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192050550.png)

1.	登录按钮： 点击请求登录
2.	转到注册按钮： 点击转到注册界面
3.	转到手机号登录按钮：点击转到手机号登录

### 手机号登录界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192113705.png)

1.	登录按钮： 点击请求登录
2.	转到注册按钮： 点击转到注册界面
3.	转到用户名登录按钮：点击转到用户名登录

### 账户优惠等级升级界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192139112.png)

1.	升级为VIP按钮：由普通成员升级为VIP
2.	升级为SVIP按钮：由普通成员升级为SVIP

### 消费者菜品列表界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192140617.png)

1.	导航栏按钮： 点击切换对应界面
2.	菜品样式图： 点击转到菜品详情窗口
3.	菜品名称：点击转到菜品详情窗口

### 消费者菜品详情窗口
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192124542.png)
1.	添加到购物车按钮：往购物车内添加一个菜品
2.	从购物车移除：从购物车内减少一个对应菜品

### 消费者购物车列表界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192141879.png)
1.	导航栏按钮： 点击切换对应界面
2.	生成订单：点击生成订单
3.	清空：点击清除购物车

### 消费者查看订单列表界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192141740.png)
1.	查看：点击跳转到查看订单界面
2.	订单状态：显示订单完成状态

### 消费者查看订单详情界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192140856.png)
1.	支付订单：点击转到第三方支付窗口
2.	取消订单： 取消生成的订单
3.  返回列表: 返回订单列表界面
4.	订单状态：显示订单完成状态

### 商家菜品列表界面  
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192144065.png)
1.	导航栏按钮： 点击切换对应界面
2.	菜品名称：点击转到菜品详情窗口

### 商家菜品注册界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192149989.png)
1.	菜品图片： 输入菜品图片的链接
2.	菜品名称：输入菜品名称
3.	菜品价格：输入菜品价格
4.	菜品简介：输入菜品简介
5.	注册菜品按钮：点击请求注册菜品

### 商家菜品详情窗口
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192156330.png)
1.	编辑菜品：点击跳转到菜品编辑窗口

### 商家菜品编辑窗口
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192157733.png)
1.	菜品图片： 输入菜品图片的链接
2.	菜品名称：输入菜品名称
3.	菜品价格：输入菜品价格
4.	菜品简介：输入菜品简介
5.	注册菜品按钮：点击请求保存菜品信息
6.  删除菜品按钮：点击删除菜品

### 商家订单列表界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192159376.png)
1.	导航栏按钮： 点击切换对应界面
2.	订单内容：点击转到订单详情窗口

### 商家订单详情窗口
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192159279.png)
1.	接单按钮： 接受订单
2.	拒单按钮：拒绝订单

### 管理员日志查看界面
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192200292.png)
1.	显示平台订单情况

### 第三方支付窗口
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501192201256.png)
1.  支付按钮：点击模拟支付成功

## 类图设计
![](https://picbed0521.oss-cn-shanghai.aliyuncs.com/blogpic/202501201231850.webp)



