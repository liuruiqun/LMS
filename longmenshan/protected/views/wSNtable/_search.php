<?php
/* @var $this WSNtableController */
/* @var $model WSNtable */
/* @var $form CActiveForm */
?>

<div class="wide form">

<?php $form=$this->beginWidget('CActiveForm', array(
	'action'=>Yii::app()->createUrl($this->route),
	'method'=>'get',
)); ?>

	<div class="row">
		<?php echo $form->label($model,'NUMSEQ'); ?>
		<?php echo $form->textField($model,'NUMSEQ'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'NODEID'); ?>
		<?php echo $form->textField($model,'NODEID',array('size'=>10,'maxlength'=>10)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'TYPE'); ?>
		<?php echo $form->textField($model,'TYPE',array('size'=>10,'maxlength'=>10)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'TYPENOTE'); ?>
		<?php echo $form->textField($model,'TYPENOTE',array('size'=>10,'maxlength'=>10)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'VALUE'); ?>
		<?php echo $form->textField($model,'VALUE'); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'SENDTIME'); ?>
		<?php echo $form->textField($model,'SENDTIME',array('size'=>20,'maxlength'=>20)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'RECVTIME'); ?>
		<?php echo $form->textField($model,'RECVTIME',array('size'=>20,'maxlength'=>20)); ?>
	</div>

	<div class="row">
		<?php echo $form->label($model,'LOCALTION'); ?>
		<?php echo $form->textField($model,'LOCALTION',array('size'=>10,'maxlength'=>10)); ?>
	</div>

	<div class="row buttons">
		<?php echo CHtml::submitButton('Search'); ?>
	</div>

<?php $this->endWidget(); ?>

</div><!-- search-form -->