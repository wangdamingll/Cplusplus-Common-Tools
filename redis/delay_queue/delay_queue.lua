local zset_key = 'delay-queue'
local min_score = 0
local max_score = '+inf'
local offset = 0
local limit = 1

local status, type = next(redis.call('TYPE', zset_key))
if status ~= nil and status == 'ok' then
    if type == 'zset' then
        local list = redis.call('ZRANGEBYSCORE', zset_key, min_score, max_score, 'LIMIT', offset, limit)
        if list ~= nil and #list > 0 then
            local value = list[1]
            redis.call('ZREM', zset_key, value)
            return value
        end
    end
end
return nil