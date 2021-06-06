package eg.edu.asu.eng.gpstrackingsystem;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.math.BigInteger;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    public final static String MODULE_MAC = "00:18:E4:40:00:06"; // our Module Mac Address
    public final static int REQUEST_ENABLE_BT = 1; //any number to send intent
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); // Standard number
    private static final int BEEB_TURN_ON = 15;
    private static final int BEEB_TURN_OFF = 20;

    private BluetoothAdapter bta;
    private BluetoothSocket mmSocket;
    private BluetoothDevice mmDevice;
    private BluetoothThread bluetoothThread = null;
    private double cur_lat = -1, cur_lon = -1;

    Button beebButton, locationButton;
    TextView locationTextView;
    boolean beebFlag = false;
    public Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.i("[BLUETOOTH]", "Sarting Main Tread");
        locationTextView =findViewById(R.id.locationTextView);
        locationButton = findViewById(R.id.locationButton);
        beebButton = findViewById(R.id.beebButton);

        Log.i("[BLUETOOTH]", "Setting Buttons Listeners");
        beebButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("[BLUETOOTH]", "Sending beeb signal");
                if (mmSocket.isConnected() && bluetoothThread != null) { //if we have connection to the bluetoothmodule
                    if (!beebFlag) {
                        bluetoothThread.write(BigInteger.valueOf(BEEB_TURN_ON).toByteArray());
                        beebFlag = true;
                    } else {
                        bluetoothThread.write(BigInteger.valueOf(BEEB_TURN_OFF).toByteArray());
                        beebFlag = false;
                    }
                } else {
                    Toast.makeText(MainActivity.this, "Something went wrong", Toast.LENGTH_LONG).show();
                }
            }
        });

        locationButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("[BLUETOOTH]", "Reading current coordinates");
                if(cur_lat > 0 && cur_lon > 0) {
                    locationTextView.setText("Current Latitude and Longitude\n" + cur_lat + "\n" + cur_lon + "\n");
                }
                else{
                    locationTextView.setText("Invaled Data");
                }
            }
        });

        bta = BluetoothAdapter.getDefaultAdapter();

        //Check if bluetooth is not enabled
        //Create intent for the user to turn it on
        if(!bta.isEnabled()){
            Intent enableBTIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBTIntent, REQUEST_ENABLE_BT);
        }else{
            initiateBluetoothProcess();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(resultCode == RESULT_OK && requestCode == REQUEST_ENABLE_BT){
            initiateBluetoothProcess();
        }
    }

    private void initiateBluetoothProcess(){
        if(!bta.isEnabled()){
            Log.e("[BLUETOOTH]", "Cannot open Bluetooth");
            return;
        }

        //attempt to connect to bluetooth module
        Log.i("[BLUETOOTH]", "attempt to connect to bluetooth module");
        BluetoothSocket tmp = null;
        mmDevice = bta.getRemoteDevice(MODULE_MAC);

        //create socket
        try {
            tmp = mmDevice.createRfcommSocketToServiceRecord(MY_UUID);
            mmSocket = tmp;
            mmSocket.connect();
            Log.i("[BLUETOOTH]","Connected to: "+mmDevice.getName());
        }catch(IOException e){
            try{mmSocket.close();}catch(IOException c){return;}
        }

        Log.i("[BLUETOOTH]", "Creating handler");
        mHandler = new Handler(Looper.getMainLooper()){
            @Override
            public void handleMessage(@NonNull Message msg) {
                //super.handleMessage(msg);
                if(msg.what == BluetoothThread.COORDINATE_UPDATE){
                    try {
                        String[] tmp = ((String) msg.obj).split("&");
                        cur_lat = Double.parseDouble(tmp[0]);
                        cur_lon = Double.parseDouble(tmp[1]);
                    }
                    catch (Exception e){
                    }
                }
            }
        };

        Log.i("[BLUETOOTH]", "Creating and running Thread");
        bluetoothThread = new BluetoothThread(mmSocket,mHandler);
        bluetoothThread.start();

    }
}