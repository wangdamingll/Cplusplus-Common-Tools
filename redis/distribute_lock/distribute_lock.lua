local del_key = KEYS[1]
local del_arg = ARGV[1]

if redis.call("get",del_key) == del_arg then
    return redis.call("del",del_key)
else
    return 0
end