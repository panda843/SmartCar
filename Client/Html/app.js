(function () {
    var api_url = "http://127.0.0.1:5123";

    var app = angular.module('SmartCar',[]);

    app.factory('Cookie', function() {  
        var Cookie = {};
        Cookie.setCookie = function(name,value){
            var Days = 30;
            var exp = new Date();
            exp.setTime(exp.getTime() + Days*24*60*60*1000);
            document.cookie = name + "="+ escape (value) + ";expires=" + exp.toGMTString() + ";path=/";;
        }
        Cookie.getCookie = function(name){
            var arr,reg=new RegExp("(^| )"+name+"=([^;]*)(;|$)");
            if(arr=document.cookie.match(reg)){
                return unescape(arr[2]);
            }else{
                return null; 
            }
        }
        Cookie.delCookie = function (name){
            var exp = new Date();
            exp.setTime(exp.getTime() - 1);
            var cval=getCookie(name);
            if(cval!=null)
            document.cookie= name + "="+cval+";expires="+exp.toGMTString();
        }
        return Cookie;
    }); 

    app.factory('User', function() {  
        var User = {};  
        var username;
        var nickname;
        var head;
        var id;
        User.setUserName = function(name){
            username = name;
        }
        User.setNickName = function(nick){
            nickname = nick;
        }
        User.setHead = function(new_head){
            head = new_head;
        }
        User.setId = function(new_id){
            id = new_id;
        }
        User.getUserName = function(){
            return username;
        }
        User.getNickName = function(){
            return nickname;
        }
        User.getHead = function(){
            return head;
        }
        User.getId = function(){
            return id;
        }
        User.isLogin = function(){
            
        }
        return User;  
    }); 

    app.controller('IndexCtl',function($scope,Cookie) {
        var token = Cookie.getCookie('token');
        if(!token){
            window.location.href=window.location.protocol+"//"+document.domain+"/login.html";
        }
    });
    app.controller('LoginCtl',function($scope,Cookie) {
        var token = Cookie.getCookie('token');
        if(token){
            window.location.href=window.location.protocol+"//"+document.domain+"/index.html";
        }
        $scope.submit = function(){
          $.post(api_url+"/user/login",{username:$scope.username,password:$scope.password},function(result){
              console.log(result);
            });  
        }
    });
}());