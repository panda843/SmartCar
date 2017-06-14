var Route = {};

Route.Construct = function(app){
    app.factory('Route', function() {  
        var Route = {};
        //跳转页面
        Route.Redirect = function (url){
            window.location.href=window.location.protocol+"//"+document.domain+"/"+url+".html";
        }
        //获取跳转页面URL
        Route.getRedirectUrl = function(url){
            return window.location.protocol+"//"+document.domain+"/"+url+".html";
        }
        return Route;
    });
}

