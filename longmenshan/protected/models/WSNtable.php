<?php

/**
 * This is the model class for table "WSNtable".
 *
 * The followings are the available columns in table 'WSNtable':
 * @property integer $NUMSEQ
 * @property string $NODEID
 * @property string $TYPE
 * @property string $TYPENOTE
 * @property double $VALUE
 * @property string $SENDTIME
 * @property string $RECVTIME
 * @property string $LOCALTION
 */
class WSNtable extends CActiveRecord
{
	/**
	 * @return string the associated database table name
	 */
	public function tableName()
	{
		return 'WSNtable';
	}

	/**
	 * @return array validation rules for model attributes.
	 */
	public function rules()
	{
		// NOTE: you should only define rules for those attributes that
		// will receive user inputs.
		return array(
			array('NODEID, TYPE, TYPENOTE, VALUE, SENDTIME, RECVTIME, LOCALTION', 'required'),
			array('VALUE', 'numerical'),
			array('NODEID, TYPE, TYPENOTE, LOCALTION', 'length', 'max'=>10),
			array('SENDTIME, RECVTIME', 'length', 'max'=>20),
			// The following rule is used by search().
			// @todo Please remove those attributes that should not be searched.
			array('NUMSEQ, NODEID, TYPE, TYPENOTE, VALUE, SENDTIME, RECVTIME, LOCALTION', 'safe', 'on'=>'search'),
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
			'NUMSEQ' => '数据ID',
			'NODEID' => '节点ID',
			'TYPE' => '节点',
			'TYPENOTE' => '节点类型',
			'VALUE' => '数值',
			'SENDTIME' => '数据发送时间',
			'RECVTIME' => '数据接收时间',
			'LOCALTION' => 'Localtion',
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

		$criteria->compare('NUMSEQ',$this->NUMSEQ);
		$criteria->compare('NODEID',$this->NODEID,true);
		$criteria->compare('TYPE',$this->TYPE,true);
		$criteria->compare('TYPENOTE',$this->TYPENOTE,true);
		$criteria->compare('VALUE',$this->VALUE);
		$criteria->compare('SENDTIME',$this->SENDTIME,true);
		$criteria->compare('RECVTIME',$this->RECVTIME,true);
		$criteria->compare('LOCALTION',$this->LOCALTION,true);

		return new CActiveDataProvider($this, array(
			'criteria'=>$criteria,
		));
	}

	/**
	 * Returns the static model of the specified AR class.
	 * Please note that you should have this exact method in all your CActiveRecord descendants!
	 * @param string $className active record class name.
	 * @return WSNtable the static model class
	 */
	public static function model($className=__CLASS__)
	{
		return parent::model($className);
	}
}
