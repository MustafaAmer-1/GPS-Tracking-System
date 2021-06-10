package eg.edu.asu.eng.gpstrackingsystem;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    public final static String MODULE_MAC = "00:18:E4:40:00:06"; // our Module Mac Address
    public final static int REQUEST_ENABLE_BT = 1; //any number to send intent
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805f9b34fb"); // Standard number
    private static final byte BEEB_TURN_ON = 'A';
    private static final byte BEEB_TURN_OFF = 'B';

    private BluetoothAdapter bta;
    private BluetoothSocket mmSocket;
    private BluetoothDevice mmDevice;
    private BluetoothThread bluetoothThread = null;
    private BroadcastReceiver mReceiver = null;
    private double strt_lat = -1, strt_lon = -1;
    private double cur_lat = -1, cur_lon = -1, first_coor = 1;
    private static double totalDistance = 0;

    Button beebButton, locationButton, setDistanceButton;
    ImageButton connectionButton;
    TextView locationTextView, connectionTextView, distanceText;
    boolean beebFlag = false;
    public Handler mHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Log.i("[BLUETOOTH]", "Sarting Main Tread");
        locationTextView = findViewById(R.id.locationTextView);
        connectionTextView = findViewById(R.id.connectionTextView);
        distanceText = findViewById(R.id.distanceText);
        locationButton = findViewById(R.id.locationButton);
        beebButton = findViewById(R.id.beebButton);
        connectionButton = findViewById(R.id.connectionButton);
        setDistanceButton = findViewById(R.id.setDistance);

        Log.i("[BLUETOOTH]", "Setting Buttons Listeners");
        beebButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("[BLUETOOTH]", "Sending beeb signal");
                if (mmSocket.isConnected() && bluetoothThread != null) { //if we have connected to the bluetoothmodule
                    if (!beebFlag) {
                        bluetoothThread.write(BEEB_TURN_ON);
                        beebFlag = true;
                    } else {
                        bluetoothThread.write(BEEB_TURN_OFF);
                        beebFlag = false;
                    }
                } else {
                    Toast.makeText(MainActivity.this, "Something went wrong\nCheck Bluetooth Connection", Toast.LENGTH_LONG).show();
                }
            }
        });

        locationButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("[BLUETOOTH]", "Reading current coordinates");

                if(cur_lat > 0 && cur_lon > 0) {
                    //locationTextView.append("Current Latitude and Longitude\n" + cur_lat + "\n" + cur_lon + "\n");
                    String uri = String.format("geo:" + cur_lat + "," + cur_lon + "?q=<" + cur_lat + ">,<" + cur_lon + ">" + "(" + "GPS" + ")");
                    Uri gmmIntentUri = Uri.parse(uri);
                    Intent mapIntent = new Intent(Intent.ACTION_VIEW, gmmIntentUri);
                    startActivity(mapIntent);
                }
                else{
                    Toast.makeText(MainActivity.this, "No racent data avilable\nCheck GPS connected", Toast.LENGTH_LONG).show();
                    //locationTextView.append("Invaled Data");
                }
            }
        });

        setDistanceButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.i("[BLUETOOTH]", "Sending Target Distance");
                if (mmSocket.isConnected() && bluetoothThread != null) { //if we have connected to the bluetoothmodule
                    String dis = distanceText.getText().toString();
                    if(dis.length() > 0){
                        bluetoothThread.write((byte)'$');
                        for (int i = 0 ; i < dis.length() ; i++){
                            bluetoothThread.write((byte) dis.charAt(i));
                        }
                        bluetoothThread.write((byte)'&');
                        distanceText.setText("");
                        Toast.makeText(MainActivity.this, "Target Distance Updated", Toast.LENGTH_LONG).show();
                    }
                } else {
                    Toast.makeText(MainActivity.this, "Something went wrong\nCheck Bluetooth Connection", Toast.LENGTH_LONG).show();
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

        mReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (BluetoothDevice.ACTION_ACL_CONNECTED.equals(action)) {
                    Toast.makeText(MainActivity.this, "Device Connected", Toast.LENGTH_LONG).show();
                    connectionButton.setBackgroundResource(R.drawable.connected);
                    connectionTextView.setText("Connected");
                }
                else if (BluetoothDevice.ACTION_ACL_DISCONNECTED.equals(action)) {
                    Toast.makeText(MainActivity.this, "Device is Disconnected", Toast.LENGTH_LONG).show();
                    connectionButton.setBackgroundResource(R.drawable.lost_connection);
                    connectionTextView.setText("Disconnected");
                }
            }
        };

        IntentFilter blueFilter = new IntentFilter();
        blueFilter.addAction(BluetoothDevice.ACTION_ACL_CONNECTED);
        blueFilter.addAction(BluetoothDevice.ACTION_ACL_DISCONNECTED);
        this.registerReceiver(mReceiver, blueFilter);

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if(resultCode == RESULT_OK && requestCode == REQUEST_ENABLE_BT){
            initiateBluetoothProcess();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

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
                        bluetoothThread.write((byte)'*'); // echo to let the boart continue it's work
                        //locationTextView.append((String)msg.obj);
                        String[] tmp = ((String) msg.obj).split("&");
                        strt_lat = cur_lat;
                        strt_lon = cur_lon;
                        cur_lat = Double.parseDouble(tmp[0].substring(0, 3)) + Double.parseDouble(tmp[0].substring(3)) / 60;
                        cur_lon = Double.parseDouble(tmp[1].substring(0, 3)) + Double.parseDouble(tmp[1].substring(3)) / 60;
                        if(first_coor == 1) {
                            strt_lat = cur_lat;
                            strt_lon = cur_lon;
                            first_coor = 0;
                        }
                        updateTotalDistance(strt_lat, strt_lon, cur_lat, cur_lon);
                        locationTextView.setText((new DecimalFormat("####.##")).format(totalDistance));
                        //locationTextView.append("OK");
                    }
                    catch (Exception e){
                    }
                }

                else if(msg.what == BluetoothThread.CONNECTED){
                    connectionButton.setBackgroundResource(R.drawable.connected);
                    connectionTextView.setText("Connected");
                }
            }
        };

        Log.i("[BLUETOOTH]", "Creating and running Thread");
        bluetoothThread = new BluetoothThread(mmSocket,mHandler);
        bluetoothThread.start();
    }

    static double updateTotalDistance(double lat1, double lon1,
                                      double lat2, double lon2){
        // distance between latitudes and longitudes
        double dLat = Math.toRadians(lat2 - lat1);
        double dLon = Math.toRadians(lon2 - lon1);

        // convert to radians
        lat1 = Math.toRadians(lat1);
        lat2 = Math.toRadians(lat2);

        // apply formulae
        double a = Math.pow(Math.sin(dLat / 2), 2) +
                Math.pow(Math.sin(dLon / 2), 2) *
                        Math.cos(lat1) *
                        Math.cos(lat2);
        double rad = 6371;
        double c = 2 * Math.asin(Math.sqrt(a));
        totalDistance += rad * c;
        return totalDistance;
    }
}