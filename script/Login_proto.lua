-- do not edit this file manully

require "script/CommandMap"

local m = CommandMap.m
local M = CommandMap.M
local R = CommandMap.Register

if Login == nil then Login = {} end

Login.ReqZoneLogin = 1568224378
Login.t_LoginFL_OK = 1089733841
Login.t_NewLoginSession = 1142697146
Login.PlayerLogin = 827721786
Login.t_NewSession_Session = 1913027674
Login.t_GYList_FL = 166467500
Login.stUserVerifyVerCmd = 1262396521
Login.stUserRequestLoginCmd = 1588675000
Login.stServerReturnLoginFailedCmd = 85552657
Login.stServerReturnLoginSuccessCmd = 1045104084

R(Login.ReqZoneLogin,"Login.ReqZoneLogin")
R(Login.t_LoginFL_OK,"Login.t_LoginFL_OK")
R(Login.t_NewLoginSession,"Login.t_NewLoginSession")
R(Login.PlayerLogin,"Login.PlayerLogin")
R(Login.t_NewSession_Session,"Login.t_NewSession_Session")
R(Login.t_GYList_FL,"Login.t_GYList_FL")
R(Login.stUserVerifyVerCmd,"Login.stUserVerifyVerCmd")
R(Login.stUserRequestLoginCmd,"Login.stUserRequestLoginCmd")
R(Login.stServerReturnLoginFailedCmd,"Login.stServerReturnLoginFailedCmd")
R(Login.stServerReturnLoginSuccessCmd,"Login.stServerReturnLoginSuccessCmd")

