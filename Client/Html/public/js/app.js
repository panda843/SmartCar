(function () {
    var api_url = "http://www.smartcar.com";

    var app = angular.module('SmartCar',[]);

    app.factory('Route', function() {  
        var Route = {};
        Route.Redirect = function (url){
            window.location.href=window.location.protocol+"//"+document.domain+"/"+url+".html";
        }
        Route.getRedirectUrl = function(url){
            return window.location.protocol+"//"+document.domain+"/"+url+".html";
        }
        return Route;
    });

    app.factory('Cookie', function() {  
        var Cookie = {};
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
        Cookie.clearCookie = function(){
            var keys=document.cookie.match(/[^ =;]+(?=\=)/g); 
            if (keys) { 
            for (var i = keys.length; i--;) 
                document.cookie=keys[i]+'=0;expires=' + new Date(0).toUTCString()+ ";path=/";
            } 
        }
        return Cookie;
    }); 
    app.factory('User', function(Cookie,Route) {  
        var User = {};  
        var username;
        var nickname;
        var head;
        var id;
        var token;
        User.setUserName = function(name){
            Cookie.setCookie("UserName",name);
            username = name;
        }
        User.setNickName = function(nick){
            Cookie.setCookie("UserNickName",nick);
            nickname = nick;
        }
        User.setHead = function(new_head){
            Cookie.setCookie("UserHead",new_head);
            head = new_head;
        }
        User.setId = function(new_id){
            Cookie.setCookie("UserID",new_id);
            id = new_id;
        }
        User.getUserName = function(){
            username = Cookie.getCookie("UserName");
            return username;
        }
        User.getNickName = function(){
            nickname = Cookie.getCookie("UserNickName");
            return nickname;
        }
        User.getHead = function(){
            head = Cookie.getCookie("UserHead");
            return head;
        }
        User.getId = function(){
            id = Cookie.getCookie("UserID");
            return id;
        }
        User.setToken = function(tokens){
            token = tokens;
            Cookie.setCookie("token",tokens);
        }
        User.getToken = function(){
            token = Cookie.getCookie("token");
            return token;
        }
        User.isLogin = function(){
            var token = Cookie.getCookie('token');
            return token;
        }
        User.logOut = function(){
            Cookie.clearCookie();
            Route.Redirect("login");
        }
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
    app.factory('Device', function(Cookie,User,$http,Route) {
        var Device = {};
        Device.sendKeyboardEvent = function(key){
            switch(key){
                case 119://W
                case 115://S
                case 97://A
                case 100://D
                case 105://I
                case 107://K
                case 106://J
                case 108://L
                    $http.get(api_url+"/device/keypress"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")+"&key="+key).then(function successCallback(response) {
                        console.log(response.data);
                    }).catch(User.checkRequestCallback);
                break;
                default:
                break;
            }
        }
        return Device;
    });
    app.controller('ControlCtl',function($scope,$document,Device,User,Cookie,Route,$http){
        if(!User.isLogin()){ Route.Redirect("login"); }
        $scope.url_console = Route.getRedirectUrl("index");
        $scope.nickname = User.getNickName();
        $scope.logout = User.logOut;
        //设置基本信息
        $http.get(api_url+"/device/info"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")).then(function successCallback(response) {
            $scope.mem_total = response.data.data.mem_total;
            $scope.mem_used = response.data.data.mem_used;
            $scope.disk_total = response.data.data.disk_total;
            $scope.disk_used = response.data.data.disk_used;
        }).catch(User.checkRequestCallback);
        //按键绑定
        $document.bind("keypress", function(event) {
            $scope.$apply(function (){
                var keycode = window.event?event.keyCode:event.which;
                Device.sendKeyboardEvent(keycode);
            })
        });
        //相机开关
        $scope.setCameraPower = function(){
            $http.get(api_url+"/camera/power"+"?token="+User.getToken()+"&sockfd="+Cookie.getCookie("control_sockfd")).then(function successCallback(response) {
                console.log(response.data.data);
            }).catch(User.checkRequestCallback);
        }
    });
    app.controller('IndexCtl',function($scope,User,Route,Cookie,$http) {
        if(!User.isLogin()){ Route.Redirect("login"); }
        $scope.url_console = Route.getRedirectUrl("index");
        $scope.nickname = User.getNickName();
        $scope.logout = User.logOut;
        $http.get(api_url+"/device/list"+"?token="+User.getToken()).then(function successCallback(response) {
            for(var index in response.data.data){
                var online = response.data.data[index].online;
                var status = response.data.data[index].status;
                response.data.data[index].online = (online == 1) ? "在线":"离线";
                response.data.data[index].status = (status == 1) ? "正常":"异常";
                response.data.data[index].control = (online == 1) ? (status == 1) ? true:false:false;
            } 
            $scope.deviceList = response.data.data;
        }).catch(User.checkRequestCallback);
        
        $scope.redirectControl = function(device_id,sockfd){
            Cookie.setCookie("control_device_id",device_id);
            Cookie.setCookie("control_sockfd",sockfd);
            console.log(sockfd);
            Route.Redirect("control");
        }
    });
    app.controller('LoginCtl',function($scope,$document,User,Route,$http) {
        if(User.isLogin()){
            Route.Redirect("index");
        }
        var sendLoginPostData = function(){
            $scope.success = true;
            $scope.error = true;
            var name = String($scope.username);
            if(name.length <= 0 || $scope.username == undefined){
                $scope.error = false;
                $scope.message = "账号不能为空";
                return;
            }
            var pass = String($scope.password);
            if(pass.length <= 0 || $scope.password == undefined){
                $scope.error = false;
                $scope.message = "密码不能为空";
                return;
            }
            $http({
                method:'POST',
                url:api_url+"/user/login",
                params:{username:$scope.username,password:$scope.password}
            }).then(function successCallback(response) {
                if(response.data.status){
                    $scope.message = "登录成功";
                    $scope.success = false;
                    User.setToken(response.data.data.token);
                    User.setId(response.data.data.user_id);
                    User.setUserName($scope.username);
                    User.setNickName(response.data.data.nickname);
                    User.setHead(response.data.data.head);
                    Route.Redirect("index");
                }else{
                    $scope.error = false;
                    $scope.message = response.data.message;
                }
            }).catch(User.checkRequestCallback);
        }
        $document.bind("keypress", function(event) {
            $scope.$apply(function (){
                var keycode = window.event?event.keyCode:event.which;
                if(keycode==13){
                    sendLoginPostData();
                }
            })
        });
        $scope.submit = function(){
            sendLoginPostData();
        }
    });
}());

var AES = {  
    sbox: [0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16],  
    Invsbox: [0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73, 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d],  
    rcon: [[0x00, 0x00, 0x00, 0x00], [0x01, 0x00, 0x00, 0x00], [0x02, 0x00, 0x00, 0x00], [0x04, 0x00, 0x00, 0x00], [0x08, 0x00, 0x00, 0x00], [0x10, 0x00, 0x00, 0x00], [0x20, 0x00, 0x00, 0x00], [0x40, 0x00, 0x00, 0x00], [0x80, 0x00, 0x00, 0x00], [0x1b, 0x00, 0x00, 0x00], [0x36, 0x00, 0x00, 0x00]],  
    cipher: function(input, w) {  
        var nb = 4;  
        var nr = w.length / nb - 1;  
        var state = [new Array(nb), new Array(nb), new Array(nb), new Array(nb)];  
        var output = new Array(4 * nb);  
        var i, round;  
        for (i = 0; i < input.length; i++) {  
            state[i % 4][Math.floor(i / 4)] = input[i];  
        }  
        this.addRoundKey(state, w, 0, nb);  
        for (round = 1; round < nr; round++) {  
            this.subBytes(state, nb, 0);  
            this.shiftRows(state, nb, 0);  
            this.mixColumns(state, nb, 0);  
            this.addRoundKey(state, w, round, nb);  
        }  
        this.subBytes(state, nb, 0);  
        this.shiftRows(state, nb, 0);  
        this.addRoundKey(state, w, nr, nb);  
        for (i = 0; i < output.length; i++) {  
            output[i] = state[i % 4][Math.floor(i / 4)];  
        }  
        return output;  
    },  
    Invcipher: function(input, w) {  
        var nb = 4;  
        var nr = w.length / nb - 1;  
        var state = [new Array(nb), new Array(nb), new Array(nb), new Array(nb)];  
        var output = new Array(4 * nb);  
        var i, round;  
        for (i = 0; i < input.length; i++) {  
            state[i % 4][Math.floor(i / 4)] = input[i];  
        }  
        this.addRoundKey(state, w, nr, nb);  
        for (round = nr - 1; round >= 1; round--) {  
            this.shiftRows(state, nb, 1);  
            this.subBytes(state, nb, 1);  
            this.addRoundKey(state, w, round, nb);  
            this.mixColumns(state, nb, 1);  
        }  
        this.shiftRows(state, nb, 1);  
        this.subBytes(state, nb, 1);  
        this.addRoundKey(state, w, round, nb);  
        for (i = 0; i < output.length; i++) {  
            output[i] = state[i % 4][Math.floor(i / 4)];  
        }  
        return output;  
    },  
    subBytes: function(state, nb, type) {  
        var r, c;  
        var type = type || 0;  
        var tempBox = type === 0 ? this.sbox: this.Invsbox;  
        for (c = 0; c < nb; c++) {  
            for (r = 0; r < 4; r++) {  
                state[r][c] = tempBox[state[r][c]];  
            }  
        }  
    },  
    shiftRows: function(state, nb, type) {  
        var temp = new Array(nb);  
        var type = type || 0;  
        var r, c;  
        type = type === 0 ? 1 : -1;  
        for (r = 1; r < 4; r++) {  
            for (c = 0; c < nb; c++) {  
                temp[c] = state[r][(c + r * type + nb) % nb];  
            }  
            for (c = 0; c < 4; c++) {  
                state[r][c] = temp[c];  
            }  
        }  
    },  
    mixColumns: function(state, nb, type) {  
        var r, c, i;  
        var t = new Array(nb);  
        var n = [[0x02, 0x03, 0x01, 0x01], [0x0e, 0x0b, 0x0d, 0x09]];  
        for (c = 0; c < nb; c++) {  
            for (r = 0; r < 4; r++) {  
                t[r] = state[r][c];  
            }  
            for (r = 0; r < 4; r++) {  
                state[r][c] = 0;  
                for (i = 0; i < 4; i++) {  
                    state[r][c] ^= this.FFmul(n[type][i], t[(r + i) % 4]);  
                }  
            }  
        }  
    },  
    FFmul: function(a, b) {  
        var bw = new Array(4);  
        var res = 0;  
        var i;  
        bw[0] = b;  
        for (i = 1; i < 4; i++) {  
            bw[i] = bw[i - 1] << 1;  
            if (bw[i - 1] & 0x80) {  
                bw[i] ^= 0x11b;  
            }  
        }  
        for (i = 0; i < 4; i++) {  
            if ((a >> i) & 0x01) {  
                res ^= bw[i];  
            }  
        }  
        return res;  
    },  
    addRoundKey: function(state, w, round, nb) {  
        var r, c;  
        for (c = 0; c < nb; c++) {  
            for (r = 0; r < 4; r++) {  
                state[r][c] ^= w[round * 4 + c][r];  
            }  
        }  
    },  
    keyExpansion: function(key) {  
        var nk = key.length / 4;  
        var nb = 4;  
        var nr = nk + 6;  
        var w = new Array(nb * (nr + 1));  
        var temp = new Array(4);  
        var i, j;  
        for (i = 0; i < nk; i++) {  
            w[i] = [key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]];  
        }  
        for (i = nk; i < w.length; i++) {  
            w[i] = new Array(4);  
            for (j = 0; j < 4; j++) {  
                temp[j] = w[i - 1][j];  
            }  
            if (i % nk === 0) {  
                this.rotWord(temp);  
                this.subWord(temp);  
                for (j = 0; j < 4; j++) {  
                    temp[j] ^= AES.rcon[i / nk][j];  
                }  
            } else if (nk > 6 && i % nk === 4) {  
                this.subWord(temp);  
            }  
            for (j = 0; j < 4; j++) {  
                w[i][j] = w[i - nk][j] ^ temp[j];  
            }  
        }  
        return w;  
    },  
    rotWord: function(w) {  
        var temp = w[0];  
        var i;  
        for (i = 0; i < 3; i++) {  
            w[i] = w[i + 1];  
        }  
        w[3] = temp;  
    },  
    subWord: function(w) {  
        var i;  
        for (i = 0; i < 4; i++) {  
            w[i] = this.sbox[w[i]];  
        }  
    }  
};  
AES.Crypto = function(key) {  
    this.String2Array = function(input, block) {  
        var i;  
        if (block != null) {  
            var output = new Array(16);  
            for (i = 0; i < 16; i++) {  
                output[i] = input.charCodeAt(16 * block + i) || 0;  
            }  
        } else {  
            var realLength = input.length <= 16 ? 16 : input.length <= 24 ? 24 : 32;  
            var output = new Array(realLength);  
            for (i = 0; i < realLength; i++) {  
                output[i] = input.charCodeAt(i) || 0;  
            }  
        }  
        return output;  
    }  
    this.Byte2Char = function(input) {  
        var i;  
        var output = '';  
        for (i = 0; i < input.length / 2; i++) {  
            output += String.fromCharCode(parseInt(input[i * 2], 16) * 16 + parseInt(input[i * 2 + 1], 16));  
        }  
        return output;  
    }  
    this.encrypt = function(input) {  
        var blockCount = Math.ceil(input.length / blockSize);  
        var output = new Array();  
        var counterBlock, byteCount, offset;  
        var block, c;  
        for (block = 0; block < blockCount; block++) {  
            counterBlock = AES.cipher(this.String2Array(input, block), this.keySchedule);  
            offset = block * blockSize;  
            for (c = 0; c < 16; c++) {  
                output[offset + c] = counterBlock[c] < 16 ? ('0' + counterBlock[c].toString(16)) : counterBlock[c].toString(16);  
            }  
        }  
        return output.join("");  
    };  
    this.decrypt = function(input) {  
        var blockCount = Math.ceil(input.length / blockSize * 0.5);  
        var output = new Array();  
        var counterBlock, byteCount, offset;  
        var block, c;  
        input = this.Byte2Char(input);  
        for (block = 0; block < blockCount; block++) {  
            counterBlock = AES.Invcipher(this.String2Array(input, block), this.keySchedule);  
            offset = block*blockSize;  
            for (c = 0; c < 16 && counterBlock[c] != 0; c++) {  
                output[offset + c] = String.fromCharCode(counterBlock[c]);  
            }  
        }  
        return output.join("");  
    };  
    var blockSize = 16;  
    this.key = this.String2Array(key, null);  
    this.keySchedule = AES.keyExpansion(this.key);  
}; 

