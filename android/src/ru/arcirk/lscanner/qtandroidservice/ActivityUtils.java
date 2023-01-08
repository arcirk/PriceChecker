package ru.arcirk.lscanner.qtandroidservice;

import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.content.BroadcastReceiver;
import android.content.IntentFilter;
import java.util.Iterator;
import java.util.Set;
import android.os.Bundle;

import java.util.UUID;

public class ActivityUtils {
    private static native void sendToQt(String message);

    private static final String TAG = "ActivityUtils";
    //public static final String BROADCAST_NAME_ACTION = "ru.arcirk.litescanner.qtandroidservice.broadcast.name";
    public static final String BROADCAST_NAME_ACTION = "scan.rcv.message";
    public static final String BROADCAST_FIELD = "dataBytes";
    private static final int MIN_BARCODE_LENGTH = 8;
    private static final int MAX_BARCODE_LENGTH = 128;
    //private final String LOG_TAG = "LiteScanner";

    private String barcodeDataField = BROADCAST_FIELD;

    public void registerServiceBroadcastReceiver(Context context) {
        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(BROADCAST_NAME_ACTION);
        context.registerReceiver(serviceMessageReceiver, intentFilter);
        Log.i(TAG, "Registered broadcast receiver");
    }

    private String getBarcodeResult(Intent intent)
    {
        Bundle extras = intent.getExtras();
        assert extras != null;
        Set<String> ks = extras.keySet();
        Iterator<String> iterator = ks.iterator();
        byte[] barcode = new byte[0];
        int barcodeLength = 0;
        String barcodeStr = "";

        while (iterator.hasNext()) {

            String field = iterator.next();

            byte[] barcode_temp = intent.getByteArrayExtra(field);

            if (barcode_temp != null) {
                Log.d(TAG, field + " size = " + Integer.toString(barcode_temp.length));

                barcodeLength = barcode_temp.length;

                if (barcodeLength >= MIN_BARCODE_LENGTH && barcodeLength <= MAX_BARCODE_LENGTH){
                    barcode =  barcode_temp;
                    break;
                }
                else{
                    barcodeLength = 0;
                }
            }
            else {
                Log.d(TAG, field + " size = null");
            }
        }

        if (barcodeLength != 0)
        {
            barcodeStr = new String(barcode, 0, barcodeLength);

        }

        return barcodeStr;
    }

    private BroadcastReceiver serviceMessageReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "In OnReceive broadcast receiver");
            if (BROADCAST_NAME_ACTION.equals(intent.getAction())) {
//                String name = intent.getStringExtra("name");
//                Log.i(TAG, "Service received name: " + name);
//                String message = "Hello " + name;
//                sendToQt(message);
//                Log.i(TAG, "Service sent back message: " + message);
                    String barcodeStr;

                    if (barcodeDataField.length() == 0)
                    {
                        barcodeStr = getBarcodeResult(intent);
                    }
                    else {
                        Log.i(TAG, barcodeDataField);
                        byte[] barcode = intent.getByteArrayExtra(barcodeDataField);
                        if (barcode != null) {
                            if (barcode.length >= MIN_BARCODE_LENGTH && barcode.length <= MAX_BARCODE_LENGTH) {
                                barcodeStr = new String(barcode, 0, barcode.length);
                            }
                            else
                            {
                                barcodeStr = getBarcodeResult(intent);
                            }
                        }
                        else{
                            barcodeStr = getBarcodeResult(intent);
                        }
                    }
                    sendToQt(barcodeStr);
//                    if (mInputListener != null) {
//                        mInputListener.OnReceiveresultListener(barcodeStr);
//                    }
            }
        }
    };
}
