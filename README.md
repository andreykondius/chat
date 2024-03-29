Клиент серверное приложение, написанное на Qt-классах. 
Сервер принимает запросы от клиентов, пересылает их всем участникам. 
Клиенты могут получать и отправлять сообщения. Предусмотрена авторизация.

Сообщения передаются в формате JSON.

Типы сообщений:

Приветствие. 
Сообщение клиента:
{
    "id":1,
  "command":"HELLO"
}
Ответ сервера
{
    "id":1,
  "command":"HELLO",
  "auth_method":"plain-text"
}

Авторизация
Сообщение клиента:
{
    "id":2,
  "command":"login",
  "login":"<login>",
  "password":"<password>",
}
Ответ сервера
{
    "id":2,
  "command":"login",
  "status":"ok",
    "session":"<UUID сессии>"
}
или
{
    "id":2,
  "command":"login",
  "status":"failed",
    "message":"сообщение об ошибке"
}

Отсылка сообщения
Сообщение клиента:
{
    "id":2,
  "command":"message",
  "body":"<тело сообщения>",
  "session":"<UUID сессии>"
}
Ответ сервера
{
    "id":2,
  "command":"message_reply",
  "status":"ok",
  "client_id":"<id сообщения клиента>"
}
или (в случае если клиент не авторизован)
{
    "id":2,
  "command":"message_reply",
  "status":"failed",
    "message":"сообщение об ошибке"
}
Ответ клиента
{
    "id":2,
  "command":"message_ok",
  "session":"<UUID сессии>"
}

Отсылка сообщения с серверва
Сообщение клиента:
{
    "id":2,
  "command":"message",
  "body":"<тело сообщения>",
  "sender_login":"<login>",
  "session":"<UUID сессии>"
}
Ответ клиента
{
    "id":2,
  "command":"message_reply",
  "status":"ok",
  "client_id":"<id сообщения>"
}
или (в случае если клиент не авторизован)
{
    "id":2,
  "command":"message_reply",
  "status":"failed",
    "message":"сообщение об ошибке"
}

Проверка соединения
Сообщение клиента:
{
    "id":2,
  "command":"ping",
  "session":"<UUID сессии>"
}
Ответ сервера
{
    "id":2,
  "command":"ping_reply",
  "status":"ok",
}
или (в случае если клиент не авторизован)
{
    "id":2,
  "command":"ping_reply",
  "status":"failed",
    "message":"сообщение об ошибке"
}

Рассоединение
Сообщение клиента:
{
    "id":2,
  "command":"logout",
  "session":"<UUID сессии>"
}
Ответ сервера
{
    "id":2,
  "command":"logout_reply",
  "status":"ok",
}

В каждом сообщении передается поле id.
