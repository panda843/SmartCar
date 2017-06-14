var User = {};

User.Construct = function(app){
    app.factory('User', function(Cookie,Route) {  
        var User = {};  
        var username;
        var nickname;
        var head;
        var id;
        var token;
        //设置用户名称
        User.setUserName = function(name){
            Cookie.setCookie("UserName",name);
            username = name;
        }
        //设置用户昵称
        User.setNickName = function(nick){
            Cookie.setCookie("UserNickName",nick);
            nickname = nick;
        }
        //设置用户头像
        User.setHead = function(new_head){
            Cookie.setCookie("UserHead",new_head);
            head = new_head;
        }
        //设置用户ID
        User.setId = function(new_id){
            Cookie.setCookie("UserID",new_id);
            id = new_id;
        }
        //获取用户名称
        User.getUserName = function(){
            username = Cookie.getCookie("UserName");
            return username;
        }
        //获取用户昵称
        User.getNickName = function(){
            nickname = Cookie.getCookie("UserNickName");
            return nickname;
        }
        //获取用户头像
        User.getHead = function(){
            head = Cookie.getCookie("UserHead");
            return head;
        }
        //获取用户ID
        User.getId = function(){
            id = Cookie.getCookie("UserID");
            return id;
        }
        //设置Token
        User.setToken = function(tokens){
            token = tokens;
            Cookie.setCookie("token",tokens);
        }
        //获取Token
        User.getToken = function(){
            token = Cookie.getCookie("token");
            return token;
        }
        //检查是否登录
        User.isLogin = function(){
            var token = Cookie.getCookie('token');
            return token;
        }
        //退出登录
        User.logOut = function(){
            Cookie.clearCookie();
            Route.Redirect("login");
        }
        //检查请求返回
        User.checkRequestCallback = function(response){
            if(response.status == 502 || response.status == -1){
                alert("获取服务器数据失败");
            }else if(response.status == 401){
                alert("身份认证过期啦");
                var keys=document.cookie.match(/[^ =;]+(?=\=)/g); 
                if (keys) { 
                for (var i = keys.length; i--;) 
                    document.cookie=keys[i]+'=0;expires=' + new Date(0).toUTCString()+ ";path=/";
                } 
                window.location.href=window.location.protocol+"//"+document.domain+"/login.html";
            }else{
                alert("未知错误");
            }
        }
        return User;  
    });
};