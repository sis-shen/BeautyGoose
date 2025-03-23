-- 创建数据库
DROP DATABASE if EXISTS BeautyGoose;
DROP USER IF EXISTS btyGooseUser;
CREATE DATABASE BeautyGoose DEFAULT CHARACTER SET utf8mb4;

-- 创建用户和赋予权限
CREATE USER 'btyGooseUser'@'%' IDENTIFIED BY 'Password!1';
grant all on BeautyGoose.* to 'btyGooseUser'@'%';
flush privileges;

-- 创建表结构
use BeautyGoose;

create table account(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `uuid` varchar(32) unique,
    `phone` varchar(16) unique,
    `name` varchar(16) not null,
    `password` varchar(64) not null,
    `nickname` varchar(32),
    `icon_data` blob,
    `level` tinyint not null default 0,
    `type` tinyint not null default 0,
    `balance` decimal unsigned default 0 
)ENGINE=InnoDB;

create table dishes(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `uuid` varchar(32) unique,
    `merchant_id` varchar(32) not null,
    `merchant_name` varchar(16) not null,
    `name` varchar(16) not null,
    `icon_path` varchar(256),
    `description` text,
    `base_price` decimal(6,2),
    `price_factor` float(4,2) unsigned,
    `is_delete` boolean not null default 1
)ENGINE=InnoDB;

create table orders(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time` varchar(32) not null, 
    `uuid` varchar(32) unique,
    `merchant_id` varchar(32) not null,
    `merchant_name` varchar(16) not null,
    `consumer_id` varchar(32) not null,
    `consumer_name` varchar(16) not null,
    `level` tinyint not null default 0,
    `pay` decimal(10,2) not null default 0,
    `status` tinyint not null default 0,
    `sum` int not null
)ENGINE=InnoDB;

create table orderDish(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `order_id` varchar(32) not null,
    `dish_id` varchar(32) not null,
    `merchant_id` varchar(32) not null,
    `name` varchar(16) not null,
    `dish_price` decimal(6,2),
    `count` int not null default 0
)ENGINE=InnoDB;

create table history(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `time` varchar(32) not null, 
    `uuid` varchar(32) unique,
    `merchant_id` varchar(32) not null,
    `merchant_name` varchar(16) not null,
    `consumer_id` varchar(32) not null,
    `consumer_name` varchar(16) not null,
    `level` tinyint not null default 0,
    `pay` decimal(10,2) not null default 0,
    `status` tinyint not null default 0,
    `sum` int not null
)ENGINE=InnoDB;

create table historyDish(
    `id` BIGINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    `order_id` varchar(32) not null,
    `dish_id` varchar(32) not null,
    `merchant_id` varchar(32) not null,
    `name` varchar(16) not null,
    `dish_price` decimal(6,2),
    `count` int not null default 0
)ENGINE=InnoDB;