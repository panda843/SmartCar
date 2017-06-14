var Cookie = {};

Cookie.Construct = function(app){
    app.factory('Cookie', function() {  
        var Cookie = {};
        //设置Cookie
        Cookie.setCookie = function(name,value){
            var exp = new Date();
            exp.setTime(exp.getTime() + 1*60*60*1000);
            if(name == "token"){
                document.cookie = name + "="+ escape(value) + ";expires=" + exp.toGMTString() + ";path=/";
            }else{
                var token =  Cookie.getCookie('token');
                var en = new AES.Crypto(token);    
                if(token){
                    document.cookie = en.encrypt(name) + "="+ en.encrypt(escape(value)) + ";expires=" + exp.toGMTString() + ";path=/";
                }
            }
        }
        //获取Cookie
        Cookie.getCookie = function(name){
            if(name == "token"){
               var arr,reg=new RegExp("(^| )"+name+"=([^;]*)(;|$)");
               if(arr=document.cookie.match(reg)){
                   return unescape(arr[2]);
               }else{
                   return null; 
               } 
           }else{
                var token =  Cookie.getCookie('token');
                var en = new AES.Crypto(token);
                var arr,reg=new RegExp("(^| )"+en.encrypt(name)+"=([^;]*)(;|$)");
                if(arr=document.cookie.match(reg)){
                    return en.decrypt(unescape(arr[2]));
                }else{
                    return null; 
                } 
           }
            
        }
        //删除Cookie
        Cookie.delCookie = function (name){
            var exp = new Date();
            exp.setTime(exp.getTime() - 1);
            if(name == "token"){
                var cval=Cookie.getCookie(name);
                if(cval!=null)document.cookie= name + "="+cval+";expires="+exp.toGMTString()+ ";path=/";
            }else{
                var token =  Cookie.getCookie('token');
                var en = new AES.Crypto(token);
                var cval=Cookie.getCookie(en.encrypt(name));
                if(cval!=null)document.cookie= en.encrypt(name) + "="+cval+";expires="+exp.toGMTString()+ ";path=/";
            }
        }
        //清空Cookie
        Cookie.clearCookie = function(){
            var keys=document.cookie.match(/[^ =;]+(?=\=)/g); 
            if (keys) { 
            for (var i = keys.length; i--;) 
                document.cookie=keys[i]+'=0;expires=' + new Date(0).toUTCString()+ ";path=/";
            } 
        }
        return Cookie;
    });
};