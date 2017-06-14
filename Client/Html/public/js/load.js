const Service = "Service";
const Plugin = "Plugin";
const Controller = "Controller";

var GankTools = {
    _array_service:[],
    _array_plugin:[],
    _array_controller:[],
    app:angular.module('SmartCar',[]),
    api_url:"http://www.smartcar.com"
};

GankTools.loadJS = function(name,type){
    var url;
    switch(type){
        case Service:
            url = window.location.protocol+"//"+document.domain+"/"+"public/js/"+Service+"/"+name+".js";
            this._array_service[name] = url;
            break;
        case Plugin:
            url = window.location.protocol+"//"+document.domain+"/"+"public/js/"+Plugin+"/"+name+".js";
            this._array_plugin[name] = url;
            break;
        case Controller:
            url = window.location.protocol+"//"+document.domain+"/"+"public/js/"+Controller+"/"+name+".js";
            this._array_controller[name] = url;
            break;
        default:
            console.log("loadJs "+name+" falied !");
        break;
    }
    document.write("<script language=javascript src='"+url+"'></script>");
};
GankTools.loadCSS = function(name){
    var url = window.location.protocol+"//"+document.domain+"/"+"public/css/"+name+".css";
    document.write("<link href='"+url+"' rel='stylesheet'>");
};

GankTools.loadService = function(){
    for(obj in this._array_service){
        window[obj].Construct(this.app);
    }
}
GankTools.loadController = function(name){
    for(obj in this._array_controller){
        window[obj].Construct(this.app);
    }
}

GankTools.loadJS("AES",Plugin);
GankTools.loadJS("Route",Service);
GankTools.loadJS("Video",Service);
GankTools.loadJS("Cookie",Service);
GankTools.loadJS("User",Service);

GankTools.loadJS("Login",Controller);
GankTools.loadJS("Index",Controller);
GankTools.loadJS("Control",Controller);