# Mook_UI
## Update
我會try my best finish.等待完整交成


##  Hardware
micro controller: ESP-WROOM-32 ESP-32S( WIFI+bluetooth)  

ILI9341 2.8吋 320 x 240 電阻式觸控 TFT全彩螢幕 LCD螢幕 彩色液晶 模組
BUY: https://shopee.tw/ILI9341-2.8%E5%90%8B-320-x-240-%E9%9B%BB%E9%98%BB%E5%BC%8F%E8%A7%B8%E6%8E%A7-TFT%E5%85%A8%E5%BD%A9%E8%9E%A2%E5%B9%95-LCD%E8%9E%A2%E5%B9%95-%E5%BD%A9%E8%89%B2%E6%B6%B2%E6%99%B6-%E6%A8%A1%E7%B5%84-i.656213378.24631477337

## Circuit
![image](https://github.com/sfsf100/Mook_UI/blob/main/IMAGE/DISPLAYER.png)


## Design pattern
![image](https://github.com/sfsf100/Mook_UI/blob/main/IMAGE/page_life.png)

根據頁面目前狀態，對頁面動作進行管理。例如頁面的加載，在執行onViewLoad時初始化視圖和資料；在onViewDidAppear（頁面即將顯示）執行過渡動畫，在動畫結束時顯示容器。
在有外部按鍵事件觸發後，依序執行onViewWillDisappear(頁面即將消失)、 onViewDidDisappear （頁面消失完成）、onViewDidUnload（頁面卸載完成）。

## 訊息框架
訊息框架提供資料的分發和處理。其使用訂閱發布機製完成，將HAL 層的接收到的感測器資料發布，並轉發給對應的訂閱者進行資料處理。


以GPS 為例，GPS 資料處理節點每秒讀取一次衛星數據，然後發起publish ，由訊息框架將GPS 資料推送給訂閱者。在運動資料處理節點中訂閱GPS 資料。
在收到GPS 資料之後，運動資料處理節點根據GPS 資料計算總里程，平均速度等資訊。在錶盤頁面中，拉取運動資料節點訊息，將其顯示在錶盤頁面中。

例如下圖為GPS 資料事件回呼函數:


