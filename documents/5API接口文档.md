# 美鹅外卖平台系统 - API 接口文档

---

**版本：** V2.7  
**更新日期：** 2025-06-04  
**作者：** 沈冬法  

---

## 接口说明概览

### 账户相关 Account API

| HTTP方法 | 路径                          | 接口功能描述                 |
|----------|-------------------------------|------------------------------|
| POST     | `/account/register`           | 注册新账户                   |
| POST     | `/account/login/username`     | 使用用户名登录账户           |
| POST     | `/account/login/phone`        | 使用手机号登录账户           |
| POST     | `/account/update/level`       | 更新消费者账户的优惠等级     |
| POST     | `/account/update/nickname`    | 更新账户昵称                 |

---

### 消费者相关 Consumer API

| HTTP方法 | 路径                                      | 接口功能描述                   |
|----------|-------------------------------------------|--------------------------------|
| GET      | `/consumer/order/detail/dishlist`         | 获取某订单中菜品的详细列表     |
| GET      | `/consumer/dish/list`                     | 获取可购买的菜品列表           |
| GET      | `/consumer/dish/dishInfo`                 | 查看某菜品的详情               |
| POST     | `/consumer/order/generate`                | 创建订单                       |
| GET      | `/consumer/order/list`                    | 获取消费者的全部订单列表       |
| POST     | `/consumer/order/pay/confirm`             | 完成订单支付                   |
| POST     | `/consumer/order/cancel`                  | 请求取消订单                   |

---

### 商家相关 Merchant API

| HTTP方法 | 路径                                      | 接口功能描述                   |
|----------|-------------------------------------------|--------------------------------|
| GET      | `/merchant/dish/list`                     | 获取商家菜品列表               |
| POST     | `/merchant/dish/register`                 | 注册（新增）菜品               |
| GET      | `/merchant/dish/info`                     | 获取指定菜品详情               |
| POST     | `/merchant/dish/update`                   | 更新菜品信息                   |
| POST     | `/merchant/dish/del`                      | 删除菜品                       |
| GET      | `/merchant/order/list`                    | 获取当前订单列表               |
| GET      | `/merchant/order/detail/dishlist`         | 获取某订单对应的菜品明细       |
| POST     | `/merchant/order/accept`                  | 商家接单                       |
| POST     | `/merchant/order/reject`                  | 商家拒单                       |

---

### 管理员相关 Admin API

| HTTP方法 | 路径                          | 接口功能描述           |
|----------|-------------------------------|------------------------|
| GET      | `/admin/order/list`           | 获取平台所有订单列表   |
---

## 通用规范

- **协议**：HTTP
- **数据格式**：请求/响应均为 `application/json`
~~- **认证方式**（如启用）：Token 通过 `Authorization` 头传递~~
---

## ~~接口详情~~

**以下仅为示例**

### 1. 用户注册

**URL：** `/api/account/register`  
**方法：** POST  
**说明：** 创建一个新账户

#### 请求参数

| 参数名   | 类型   | 是否必填 | 说明       |
|----------|--------|----------|------------|
| username | string | 是       | 用户名     |
| password | string | 是       | 密码（加密） |
| phone    | string | 是       | 手机号     |

#### 请求示例

```json
{
  "username": "zhangsan",
  "password": "hashed_pw",
  "phone": "13900001111"
}
```

#### 响应参数

| 参数名   | 类型    | 说明           |
|----------|---------|----------------|
| success  | boolean | 注册是否成功   |
| message  | string  | 结果说明信息   |

#### 响应示例

```json
{
  "success": true,
  "message": "注册成功"
}
```

---



## ~~错误码定义~~

| 错误码 | 描述                       | 说明                           |
|--------|----------------------------|--------------------------------|
| 4001   | 参数不合法                 | 缺失必要字段                   |
| 4002   | 商品不存在                 | 菜品已下架或ID错误             |
| 4003   | 订单不存在                 | 查询了无效的订单ID             |
| 5001   | 系统内部错误               | 服务端异常                     |
| 5002   | 支付系统暂时不可用         | 外部支付接口调用失败           |

---

## 接口测试建议

- 所有 POST 请求需 Content-Type 设置为 `application/json`
- 建议结合 Postman、Apifox 进行接口测试
- 推荐加入 Token 验证（未来计划）

---


