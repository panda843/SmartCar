var Video = {};

Video.Construct = function(app){
    app.factory('Video', function() {
        //video对象  
        var Video = {};
        //SWF控件
        videojs.options.flash.swf = "https://cdn.bootcss.com/video.js/6.0.1/video-js.swf";
        //video_id
        var object = videojs('smartVideo');
        //mime
        var mime = {'hls':'application/x-mpegURL','rtmp':'rtmp/flv'};
        //添加菜单
        Video.initVideo = function(){
            object.ready(function() {
                var menu = [{"name":"切换源","sub":[{"name":"HLS"},{"name":"RTMP"}]}];
                Video.addMenu(menu);
            });
        }
        //添加菜单
        Video.addMenu = function(data){
            $videoPanelMenu = $(".vjs-fullscreen-control");
            var html ='<div class="vjs-subs-caps-button vjs-menu-button vjs-menu-button-popup vjs-control vjs-button">';
            for(var i=0;i<data.length;i++){
                html +='<button class="vjs-subs-caps-button vjs-menu-button vjs-menu-button-popup vjs-button" type="button" aria-live="polite" title="'+data[i].name+'" aria-haspopup="true" aria-expanded="false">';
                html += '<span  class="vjs-icon-placeholder"></span><span class="vjs-control-text">'+data[i].name+'</span></button>';
                if(data[i].hasOwnProperty("sub")){
                    html += '<div class="vjs-menu"><ul class="vjs-menu-content" role="menu">';
                    for(var x=0;x<data[i].sub.length;x++){
                        var sub = data[i].sub[x];
                        html +='<li class="vjs-menu-item vjs-texttrack-settings" tabindex="-1" role="menuitem" aria-live="polite">';
                        html +='<span class="vjs-menu-item-text">'+sub.name+'</span>';
                        html +='<span class="vjs-control-text">open '+sub.name+'</span></li>';
                    }
                    html +='</ul></div>'
                }
                html +='</div>';
            }
            $videoPanelMenu.before(html);
        }
        //检查是否支持flash
        Video.isFlash = function(){
            var hasFlash = 0; //是否安装了flash
            var flashVersion = 0; //flash版本
            if (document.all) {
                var swf = new ActiveXObject('ShockwaveFlash.ShockwaveFlash');
                if (swf) {
                    hasFlash = 1;
                    VSwf = swf.GetVariable("$version");
                    flashVersion = parseInt(VSwf.split(" ")[1].split(",")[0]);
                }
            } else {
                if(navigator.plugins && navigator.plugins.length > 0) {
                    var swf = navigator.plugins["Shockwave Flash"];
                    if (swf) {
                        hasFlash = 1;
                        var words = swf.description.split(" ");
                        for (var i = 0; i < words.length; ++i) {
                            if (isNaN(parseInt(words[i]))) continue;
                            flashVersion = parseInt(words[i]);
                        }
                    }
                }
            }
            return hasFlash;
        }
        //设置播放地址
        Video.setUrl = function(url,type){
            object.src({
                src: url,
                type: mime[type]
            });
        }
        //开始播放
        Video.start = function(){
            object.play();
        }
        //暂停播放
        Video.stop = function(){
            object.pause();
        }
        //重载视频
        Video.reload = function(){
            object.load();
        }
        return Video;
    });
};