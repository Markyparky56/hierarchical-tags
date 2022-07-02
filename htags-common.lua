newoption {
    trigger = "with-exceptions",
    description = "Exceptions Always Enabled? (ON/OFF)"
}
EXCEPTIONS_ENABLED = "Off"
if _OPTIONS["with-exceptions"] ~= nil then
    EXCEPTIONS_ENABLED = "On"
end

newoption {
    trigger = "dynamic-runtime",
    description = "Should use dynamically linked runtime?"
}
STATIC_RUNTIME = "On"
if _OPTIONS["dynamic-runtime"] ~= nil then
    STATIC_RUNTIME = "Off"
end

SRC_DIR = "src/"
INCLUDE_DIR = "include/"
TESTS_DIR = "tests/"
