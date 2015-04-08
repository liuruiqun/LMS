<?php

/**
 * This is the model class for table "WSNkongyadata".
 *
 * The followings are the available columns in table 'WSNkongyadata':
 * @property integer $DID
 * @property integer $SID
 * @property string $DataTime
 * @property string $S1
 * @property string $S2
 * @property string $R1
 * @property string $R2
 * @property string $IsWarning
 */
class WSNkongyadata extends CActiveRecord
{
	/**
	 * @return string the associated database table name
	 */
	public function tableName()
	{
		return 'WSNkongyadata';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('DID', 'required'),
			array('DID, SID', 'numerical', 'integerOnly'=>true),
			array('DataTime, S1, S2, R1, R2, IsWarning', 'length', 'max'=>50),
			// The following rule is used by search().
			// @todo Please remove those attributes that should not be searched.
			array('DID, SID, DataTime, S1, S2, R1, R2, IsWarning', 'safe', 'on'=>'search'),
		);
	}

	/**
	 * @return array relational rules.
	 */
	public function relations()
	{
		// NOTE: you may need to adjust the relation name and the related
		// class name for the relations automatically generated below.
		return array(
		);
	}

	/**
	 * @return array customized attribute labels (name=>label)
	 */
	public function attributeLabels()
	{
		return array(
			'DID' => '数据ID',
			'SID' => '节点ID',
			'DataTime' => '数据接收时间',
			'S1' => 'S1',
			'S2' => 'S2',
			'R1' => '地下水位（单位：毫米）',
			'R2' => '温度（单位：摄氏度）',
			'IsWarning' => 'Is Warning',
		);
	}

	/**
	 * Retrieves a list of models based on the current search/filter conditions.
	 *
	 * Typical usecase:
	 * - Initialize the model fields with values from filter form.
	 * - Execute this method to get CActiveDataProvider instance which will filter
	 * models according to data in model fields.
	 * - Pass data provider to CGridView, CListView or any similar widget.
	 *
	 * @return CActiveDataProvider the data provider that can return the models
	 * based on the search/filter conditions.
	 */
	public function search()
	{
		// @todo Please modify the following code to remove attributes that should not be searched.

		$criteria=new CDbCriteria;

		$criteria->compare('DID',$this->DID);
		$criteria->compare('SID',$this->SID);
		$criteria->compare('DataTime',$this->DataTime,true);
		$criteria->compare('S1',$this->S1,true);
		$criteria->compare('S2',$this->S2,true);
		$criteria->compare('R1',$this->R1,true);
		$criteria->compare('R2',$this->R2,true);
		$criteria->compare('IsWarning',$this->IsWarning,true);

		return new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
		));
	}

	/**
	 * Returns the static model of the specified AR class.
	 * Please note that you should have this exact method in all your CActiveRecord descendants!
	 * @param string $className active record class name.
	 * @return WSNkongyadata the static model class
	 */
	public static function model($className=__CLASS__)
	{
		return parent::model($className);
	}
}
