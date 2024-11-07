在 MySQL 中创建用户信息通常涉及到以下几个步骤：首先登录到 MySQL 数据库，然后使用 SQL 语句来创建新用户并设置密码，接着授予用户适当的权限，并刷新权限缓存以确保更改立即生效。

步骤
  1. 登录 MySQL 数据库。
  2. 创建新用户并设置密码。
  3. 授予用户权限。
  4. 刷新权限缓存。

1. 登录 MySQL 数据库
首先，使用具有足够权限的账户登录 MySQL 数据库，通常是 root 用户。
```sh
mysql -u root -p
```
输入密码后，你会进入 MySQL 的命令行界面。

2.  创建新用户并设置密码
在 MySQL 命令行界面中，使用 CREATE USER 语句来创建新用户，并设置密码。
```sql
CREATE USER 'newuser'@'localhost' IDENTIFIED BY 'password123';
```
这条命令创建了一个名为 newuser 的用户，并将其密码设置为 password123。注意，这里的 localhost 表示用户只能从本地主机登录数据库。如果你希望用户可以从远程主机登录，可以将 localhost 替换为相应的 IP 地址或域名，例如 'newuser'@'%' 表示允许从任何主机登录。

3. 授予用户权限
根据需要，可以授予用户不同的权限。例如，授予用户对所有数据库的所有权限：
```sql
GRANT ALL PRIVILEGES ON *.* TO 'newuser'@'localhost' WITH GRANT OPTION;
```
这条命令授予了用户 newuser 对所有数据库的所有权限，并允许其授予其他用户权限。

如果你只想授予特定数据库的某些权限，可以调整 ON 后面的部分。例如，只授予 exampledb 数据库的读写权限：
```sql
GRANT SELECT, INSERT, UPDATE, DELETE ON exampledb.* TO 'newuser'@'localhost';
```

4. 刷新权限缓存
为了确保更改立即生效，需要刷新权限缓存：
```sql
FLUSH PRIVILEGES;
```

### 插入数据

```sql
INSERT INTO table_name (column1, column2, column3, ...)
VALUES (value1, value2, value3, ...);
```
参数说明：
 + table_name 是你要插入数据的表的名称。
 + column1, column2, column3, ... 是表中的列名。
 + value1, value2, value3, ... 是要插入的具体数值。

如果数据是字符型，必须使用单引号 ' 或者双引号 "，如： 'value1', "value1"。

如果你要插入所有列的数据，可以省略列名：
```sql
INSERT INTO users
VALUES (NULL,'test', 'test@runoob.com', '1990-01-01', true);
```
这里，NULL 是用于自增长列的占位符，表示系统将为 id 列生成一个唯一的值。

如果你要插入多行数据，可以在 VALUES 子句中指定多组数值：
```sql
INSERT INTO users (username, email, birthdate, is_active)
VALUES
    ('test1', 'test1@runoob.com', '1985-07-10', true),
    ('test2', 'test2@runoob.com', '1988-11-25', false),
    ('test3', 'test3@runoob.com', '1993-05-03', true);
```
以上代码将在 users 表中插入三行数据。





