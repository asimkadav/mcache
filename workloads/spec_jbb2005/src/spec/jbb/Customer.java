/* Copyright (c) 2000-2005 Standard Performance Evaluation Corporation (SPEC) * All rights reserved. * Copyright (c) 1996-2005 IBM Corporation, Inc. All rights reserved. */package spec.jbb;import java.math.BigDecimal;import java.util.*;public class Customer {    // This goes right after each class/interface statement    static final String         COPYRIGHT   = "SPECjbb2005,"                                                    + "Copyright (c) 2000-2005 Standard Performance Evaluation Corporation (SPEC),"                                                    + "All rights reserved,"                                                    + "(C) Copyright IBM Corp., 1996 - 2005"                                                    + "All rights reserved,"                                                    + "Licensed Materials - Property of SPEC";    private static final String bad_credit  = "BC";    private static final String good_credit = "GC";    private Order               lastOrder;    // required data    private String              firstName;    private String              middleName;    private String              lastName;    private Address             address;    private String              phone;    private Date                since;    private String              data;    private BigDecimal          creditLimit;    private BigDecimal          discount;    private BigDecimal          balance;    private BigDecimal          ytd;    private char                credit1;    private char                credit2;    private short               customerId;    private short               paymentCount;    private short               deliveryCount;    private byte                districtId;    private short               warehouseId;    public Customer() {        address = new Address();        since = new Date();        creditLimit = BigDecimal.valueOf(0, 2);        balance = BigDecimal.valueOf(0, 2);        ytd = BigDecimal.valueOf(0, 2);        paymentCount = 0;        deliveryCount = 0;        lastOrder = null;    }    public short getId() {        return customerId;    }    public synchronized void display() {        System.out                .println("Customer Display *********************************");        System.out.println("customerId =================> " + customerId);        System.out.println("district ID ========> " + districtId);        System.out.println("warehouse ID========> " + warehouseId);        System.out.println("firstname===========> " + firstName);        System.out.println("middlename =========> " + middleName);        System.out.println("lastname ===========> " + lastName);        address.display();        System.out                .println("END ****Customer Display *********************************");    }    public synchronized void addOrder(Order thisOrder) {        lastOrder = thisOrder;    }    public synchronized Order getLatestOrder() {        Order order = lastOrder;        return order;    }    public String getFirstName() {        return firstName;    };    public String getMiddleName() {        return middleName;    };    public String getLastName() {        return lastName;    };    public Address getAddress() {        return address;    }    public BigDecimal getDiscountRate() {        return discount;    };    public String getCreditStatus() {        String result;        if (credit1 == 'G')            result = good_credit;        else            result = bad_credit;        return result;    };    public short getWarehouseId() {        return warehouseId;    }    public byte getDistrictId() {        return districtId;    }    public synchronized BigDecimal getBalance() {        BigDecimal temp = balance;        return temp;    }    public synchronized BigDecimal getCreditLimit() {        return creditLimit;    }    public synchronized void adjustBalance(BigDecimal amount) {        balance = balance.add(amount);    }    public synchronized void increaseYTD(BigDecimal amount) {        ytd = ytd.add(amount);    }    public synchronized void incrementPaymentCount() {        ++paymentCount;    }    public synchronized void incrementDeliveryCount() {        ++deliveryCount;    }    public synchronized void updateCustomerData(String newData) {        String oldData;        // The data must be added on the left and the old data shifted right.        oldData = data;        if ((oldData.length() + newData.length()) <= 500) {            data = new String((newData + data));        }        else {            String shiftData = oldData.substring(0, (500 - newData.length()));            data = new String((newData + shiftData));        }    }    public synchronized void setUsingRandom(short inCustomerId,            short inWarehouseId, byte inDistrictId) {        customerId = inCustomerId;        districtId = inDistrictId;        warehouseId = inWarehouseId;        lastName = JBButil.create_random_last_name(inCustomerId, warehouseId);        firstName = new String(JBButil.create_random_a_string(8, 16,                warehouseId));        middleName = new String("OE");        address.setUsingRandom(warehouseId);        phone = new String(JBButil.create_random_n_string(16, 16, warehouseId));        if (JBButil.random(1, 10, warehouseId) > 1) {            credit1 = 'G';            credit2 = 'C';        }        else {            credit1 = 'B';            credit2 = 'C';        }        ;        creditLimit = BigDecimal.valueOf(5000000, 2);        float temp = JBButil.create_random_float_val_return(0.0f, 0.5000f,                0.0001f, warehouseId);        discount = new BigDecimal(temp).setScale(4, BigDecimal.ROUND_HALF_UP);        balance = BigDecimal.valueOf(-1000L, 2);        ytd = BigDecimal.valueOf(1000L, 2);        paymentCount = 1;        deliveryCount = 0;        data = new String(JBButil.create_random_a_string(300, 500, warehouseId));    }    public String getPhone() {        return phone;    }    public Date getSince() {        return since;    }    public synchronized String getCustomerDataParts() {        String temp = data;        return temp;    }}